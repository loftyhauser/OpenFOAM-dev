/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
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

#include "ignitionSite.H"
#include "Time.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::ignitionSite::ignitionSite
(
    Istream& is,
    const Time& db,
    const fvMesh& mesh
)
:
    db_(db),
    mesh_(mesh),
    ignitionSiteDict_(is),
    location_(ignitionSiteDict_.lookup<vector>("location", dimLength)),
    diameter_(ignitionSiteDict_.lookup<scalar>("diameter", dimLength)),
    time_
    (
        ignitionSiteDict_.lookup<scalar>("start", db.time().userUnits())
    ),
    duration_
    (
        ignitionSiteDict_.lookup<scalar>("duration", db.time().userUnits())
    ),
    strength_(ignitionSiteDict_.lookup<scalar>("strength")),
    timeIndex_(db_.timeIndex())
{
    // Check state of Istream
    is.check("ignitionSite::ignitionSite(Istream&)");

    findIgnitionCells(mesh_);
}


// ************************************************************************* //
