/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2023 OpenFOAM Foundation
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

Class
    Foam::displacementMotionSolver

Description
    Virtual base class for displacement motion solver

    The boundary displacement is set as a boundary condition
    on the pointDisplacement pointVectorField.

SourceFiles
    displacementMotionSolver.C

\*---------------------------------------------------------------------------*/

#ifndef displacementMotionSolver_H
#define displacementMotionSolver_H

#include "points0MotionSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class displacementMotionSolver Declaration
\*---------------------------------------------------------------------------*/

class displacementMotionSolver
:
    public points0MotionSolver
{
protected:

    // Protected data

        //- Point motion field
        mutable pointVectorField pointDisplacement_;


public:

    //- Runtime type information
    TypeName("displacementMotionSolver");


    // Constructors

        //- Construct from mesh and dictionary
        displacementMotionSolver
        (
            const word& name,
            const polyMesh&,
            const dictionary&,
            const word& type
        );

        //- Disallow default bitwise copy construction
        displacementMotionSolver(const displacementMotionSolver&) = delete;


    //- Destructor
    virtual ~displacementMotionSolver();


    // Member Functions

        //- Return reference to the point motion displacement field
        pointVectorField& pointDisplacement()
        {
            return pointDisplacement_;
        }

        //- Return const reference to the point motion displacement field
        const pointVectorField& pointDisplacement() const
        {
            return pointDisplacement_;
        }

        //- Update from another mesh using the given map
        virtual void mapMesh(const polyMeshMap&);


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const displacementMotionSolver&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
