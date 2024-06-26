/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2023 OpenFOAM Foundation
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

#include "SpalartAllmarasDDES.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace LESModels
{

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

template<class BasicMomentumTransportModel>
tmp<volScalarField::Internal>
SpalartAllmarasDDES<BasicMomentumTransportModel>::rd
(
    const volScalarField::Internal& magGradU
) const
{
    return volScalarField::Internal::New
    (
        typedName("rd"),
        min
        (
            this->nuEff()()
           /(
                max
                (
                    magGradU,
                    dimensionedScalar(magGradU.dimensions(), small)
                )
               *sqr(this->kappa_*this->y()())
            ),
            scalar(10)
        )
    );
}


template<class BasicMomentumTransportModel>
tmp<volScalarField::Internal>
SpalartAllmarasDDES<BasicMomentumTransportModel>::fd
(
    const volScalarField::Internal& magGradU
) const
{
    return volScalarField::Internal::New
    (
        typedName("fd"),
        1 - tanh(pow3(8*rd(magGradU)))
    );
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class BasicMomentumTransportModel>
tmp<volScalarField::Internal>
SpalartAllmarasDDES<BasicMomentumTransportModel>::dTilda
(
    const volScalarField::Internal& chi,
    const volScalarField::Internal& fv1,
    const volTensorField::Internal& gradU
) const
{
    return volScalarField::Internal::New
    (
        typedName("dTilda"),
        max
        (
            this->y()
          - fd(mag(gradU))
           *max
            (
                this->y()() - this->CDES_*this->delta()(),
                dimensionedScalar(dimLength, 0)
            ),
            dimensionedScalar(dimLength, small)
        )
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class BasicMomentumTransportModel>
SpalartAllmarasDDES<BasicMomentumTransportModel>::SpalartAllmarasDDES
(
    const alphaField& alpha,
    const rhoField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const viscosity& viscosity,
    const word& type
)
:
    SpalartAllmarasDES<BasicMomentumTransportModel>
    (
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        viscosity
    )
{}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace LESModels
} // End namespace Foam

// ************************************************************************* //
