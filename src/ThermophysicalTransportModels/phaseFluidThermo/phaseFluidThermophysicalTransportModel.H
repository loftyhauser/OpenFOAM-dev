/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2020-2024 OpenFOAM Foundation
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

Typedef
    Foam::phaseFluidThermophysicalTransportModel

Typedef
    Foam::laminarPhaseFluidThermophysicalTransportModel

Typedef
    Foam::RASPhaseFluidThermophysicalTransportModel

Typedef
    Foam::LESPhaseFluidThermophysicalTransportModel

Description
    Typedefs for laminar, RAS and LES thermophysical transport models
    for fluidThermo flow.

\*---------------------------------------------------------------------------*/

#ifndef phaseFluidThermophysicalTransportModel_H
#define phaseFluidThermophysicalTransportModel_H

#include "fluidThermo.H"
#include "PhaseThermophysicalTransportModel.H"
#include "laminarThermophysicalTransportModel.H"
#include "RASThermophysicalTransportModel.H"
#include "LESThermophysicalTransportModel.H"
#include "phaseCompressibleMomentumTransportModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    typedef PhaseThermophysicalTransportModel
    <
        phaseCompressible::momentumTransportModel,
        fluidThermo
    > phaseFluidThermophysicalTransportModel;

    typedef laminarThermophysicalTransportModel
    <
        phaseFluidThermophysicalTransportModel
    > laminarPhaseFluidThermophysicalTransportModel;

    typedef RASThermophysicalTransportModel
    <
        phaseFluidThermophysicalTransportModel
    > RASPhaseFluidThermophysicalTransportModel;

    typedef LESThermophysicalTransportModel
    <
        phaseFluidThermophysicalTransportModel
    > LESPhaseFluidThermophysicalTransportModel;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
