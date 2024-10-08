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

#include "porosityForce.H"
#include "porosityModel.H"
#include "fvMatrices.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(porosityForce, 0);
    addToRunTimeSelectionTable(fvModel, porosityForce, dictionary);
    addBackwardCompatibleToRunTimeSelectionTable
    (
        fvModel,
        porosityForce,
        dictionary,
        explicitPorositySource,
        "explicitPorositySource"
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fv::porosityForce::readCoeffs(const dictionary& dict)
{
    if (dict.found("UNames"))
    {
        UNames_ = wordList(dict.lookup("UNames"));
    }
    else
    {
        UNames_ = wordList(1, dict.lookupOrDefault<word>("U", "U"));
    }

    porosityPtr_.reset
    (
        porosityModel::New
        (
            name(),
            mesh(),
            dict
        ).ptr()
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::porosityForce::porosityForce
(
    const word& name,
    const word& modelType,
    const fvMesh& mesh,
    const dictionary& dict
)
:
    fvModel(name, modelType, mesh, dict),
    UNames_(),
    porosityPtr_(nullptr)
{
    readCoeffs(coeffs(dict));
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::wordList Foam::fv::porosityForce::addSupFields() const
{
    return UNames_;
}


void Foam::fv::porosityForce::addSup
(
    const volVectorField& U,
    fvMatrix<vector>& eqn
) const
{
    fvMatrix<vector> porosityEqn(eqn.psi(), eqn.dimensions());
    porosityPtr_->addResistance(porosityEqn);
    eqn -= porosityEqn;
}


void Foam::fv::porosityForce::addSup
(
    const volScalarField& rho,
    const volVectorField& U,
    fvMatrix<vector>& eqn
) const
{
    fvMatrix<vector> porosityEqn(eqn.psi(), eqn.dimensions());
    porosityPtr_->addResistance(porosityEqn);
    eqn -= porosityEqn;
}


void Foam::fv::porosityForce::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    const volVectorField& U,
    fvMatrix<vector>& eqn
) const
{
    fvMatrix<vector> porosityEqn(eqn.psi(), eqn.dimensions());
    porosityPtr_->addResistance(porosityEqn);
    eqn -= alpha*porosityEqn;
}


bool Foam::fv::porosityForce::movePoints()
{
    porosityPtr_->movePoints();

    return true;
}


void Foam::fv::porosityForce::topoChange(const polyTopoChangeMap& map)
{
    porosityPtr_->topoChange(map);
}


void Foam::fv::porosityForce::mapMesh(const polyMeshMap& map)
{
    porosityPtr_->mapMesh(map);
}


void Foam::fv::porosityForce::distribute
(
    const polyDistributionMap& map
)
{
    porosityPtr_->distribute(map);
}


bool Foam::fv::porosityForce::read(const dictionary& dict)
{
    if (fvModel::read(dict))
    {
        readCoeffs(coeffs(dict));
        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
