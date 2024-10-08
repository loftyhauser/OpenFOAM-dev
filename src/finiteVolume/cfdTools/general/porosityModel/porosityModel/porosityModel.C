/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2024 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "porosityModel.H"
#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(porosityModel, 0);
    defineRunTimeSelectionTable(porosityModel, mesh);
}


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

void Foam::porosityModel::adjustNegativeResistance(dimensionedVector& resist)
{
    scalar maxCmpt = cmptMax(resist.value());

    if (maxCmpt < 0)
    {
        FatalErrorInFunction
            << "Cannot have all resistances set to negative, resistance = "
            << resist
            << exit(FatalError);
    }
    else
    {
        maxCmpt = max(0, maxCmpt);
        vector& val = resist.value();
        for (label cmpt = 0; cmpt < vector::nComponents; cmpt++)
        {
            if (val[cmpt] < 0)
            {
                val[cmpt] *= -maxCmpt;
            }
        }
    }
}


Foam::label Foam::porosityModel::fieldIndex(const label i) const
{
    label index = 0;
    if (!coordSys_.R().uniform())
    {
        index = i;
    }
    return index;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::porosityModel::porosityModel
(
    const word& name,
    const fvMesh& mesh,
    const dictionary& dict,
    const dictionary& coeffDict,
    const word& cellZoneName
)
:
    regIOobject
    (
        IOobject
        (
            name,
            mesh.time().name(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        )
    ),
    name_(name),
    mesh_(mesh),
    zoneName_
    (
        cellZoneName != word::null
      ? cellZoneName
      : dict.lookup<word>("cellZone")
    ),
    coordSys_(coordinateSystem::New(mesh, coeffDict))
{
    Info<< "    creating porous zone: " << zoneName_ << endl;

    if (mesh_.cellZones().findIndex(zoneName_) == -1)
    {
        FatalErrorInFunction
            << "cannot find porous cellZone " << zoneName_
            << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::porosityModel::~porosityModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::vectorField> Foam::porosityModel::porosityModel::force
(
    const volVectorField& U,
    const volScalarField& rho,
    const volScalarField& mu
) const
{
    tmp<vectorField> tforce(new vectorField(U.size(), Zero));
    this->calcForce(U, rho, mu, tforce.ref());

    return tforce;
}


void Foam::porosityModel::addResistance(fvVectorMatrix& UEqn)
{
    this->correct(UEqn);
}


void Foam::porosityModel::addResistance
(
    const fvVectorMatrix& UEqn,
    volTensorField& AU,
    bool correctAUprocBC
)
{
    this->correct(UEqn, AU);

    if (correctAUprocBC)
    {
        // Correct the boundary conditions of the tensorial diagonal to
        // ensure processor boundaries are correctly handled when AU^-1 is
        // interpolated for the pressure equation.
        AU.correctBoundaryConditions();
    }
}


bool Foam::porosityModel::movePoints()
{
    calcTransformModelData();

    return true;
}


void Foam::porosityModel::topoChange(const polyTopoChangeMap& map)
{
    calcTransformModelData();
}


void Foam::porosityModel::mapMesh(const polyMeshMap& map)
{
    calcTransformModelData();
}


void Foam::porosityModel::distribute
(
    const polyDistributionMap& map
)
{
    calcTransformModelData();
}


bool Foam::porosityModel::read(const dictionary& dict)
{
    dict.lookup("cellZone") >> zoneName_;

    return true;
}


// ************************************************************************* //
