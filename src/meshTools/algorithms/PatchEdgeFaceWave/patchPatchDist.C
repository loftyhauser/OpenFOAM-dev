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

#include "patchPatchDist.H"
#include "PatchEdgeFaceWave.H"
#include "syncTools.H"
#include "polyMesh.H"
#include "patchEdgeFacePoint.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::patchPatchDist::patchPatchDist
(
    const polyPatch& patch,
    const labelHashSet& nbrPatchIDs
)
:
    patch_(patch),
    nbrPatchIndices_(nbrPatchIDs),
    nUnset_(0)
{
    patchPatchDist::correct();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::patchPatchDist::~patchPatchDist()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::patchPatchDist::correct()
{
    // Mark all edge connected to a nbrPatch.
    label nBnd = 0;
    forAllConstIter(labelHashSet, nbrPatchIndices_, iter)
    {
        label nbrPatchi = iter.key();
        const polyPatch& nbrPatch = patch_.boundaryMesh()[nbrPatchi];
        nBnd += nbrPatch.nEdges()-nbrPatch.nInternalEdges();
    }

    // Mark all edges. Note: should use HashSet but have no syncTools
    // functionality for these.
    EdgeMap<label> nbrEdges(2*nBnd);

    forAllConstIter(labelHashSet, nbrPatchIndices_, iter)
    {
        label nbrPatchi = iter.key();
        const polyPatch& nbrPatch = patch_.boundaryMesh()[nbrPatchi];
        const labelList& nbrMp = nbrPatch.meshPoints();

        for
        (
            label edgei = nbrPatch.nInternalEdges();
            edgei < nbrPatch.nEdges();
            edgei++
        )
        {
            const edge& e = nbrPatch.edges()[edgei];
            const edge meshE = edge(nbrMp[e[0]], nbrMp[e[1]]);
            nbrEdges.insert(meshE, nbrPatchi);
        }
    }


    // Make sure these boundary edges are marked everywhere.
    syncTools::syncEdgeMap
    (
        patch_.boundaryMesh().mesh(),
        nbrEdges,
        maxEqOp<label>()
    );


    // Data on all edges and faces
    List<patchEdgeFacePoint> allEdgeInfo(patch_.nEdges());
    List<patchEdgeFacePoint> allFaceInfo(patch_.size());

    // Initial seed
    label nBndEdges = patch_.nEdges() - patch_.nInternalEdges();
    DynamicList<label> initialEdges(2*nBndEdges);
    DynamicList<patchEdgeFacePoint> initialEdgesInfo(2*nBndEdges);


    // Seed all my edges that are also nbrEdges

    const labelList& mp = patch_.meshPoints();

    for
    (
        label edgei = patch_.nInternalEdges();
        edgei < patch_.nEdges();
        edgei++
    )
    {
        const edge& e = patch_.edges()[edgei];
        const edge meshE = edge(mp[e[0]], mp[e[1]]);
        EdgeMap<label>::const_iterator edgeFnd = nbrEdges.find(meshE);
        if (edgeFnd != nbrEdges.end())
        {
            initialEdges.append(edgei);
            initialEdgesInfo.append
            (
                patchEdgeFacePoint
                (
                    e.centre(patch_.localPoints()),
                    0.0
                )
            );
        }
    }


    // Walk
    PatchEdgeFaceWave<primitivePatch, patchEdgeFacePoint> calc
    (
        patch_.boundaryMesh().mesh(),
        patch_,
        initialEdges,
        initialEdgesInfo,
        allEdgeInfo,
        allFaceInfo,
        returnReduce(patch_.nEdges(), sumOp<label>())
    );


    // Extract into *this
    setSize(patch_.size());
    nUnset_ = 0;
    forAll(allFaceInfo, facei)
    {
        if (allFaceInfo[facei].valid(calc.data()))
        {
            operator[](facei) =  Foam::sqrt(allFaceInfo[facei].distSqr());
        }
        else
        {
            nUnset_++;
        }
    }
}


// ************************************************************************* //
