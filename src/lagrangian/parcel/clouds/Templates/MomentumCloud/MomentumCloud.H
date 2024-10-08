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

Class
    Foam::MomentumCloud

Description
    Templated base class for momentum cloud

    - cloud function objects

    - particle forces, e.g.
      - buoyancy
      - drag
      - pressure gradient
      - ...

    - sub-models:
      - dispersion model
      - injection model
      - patch interaction model
      - stochastic collision model
      - surface film model

SourceFiles
    MomentumCloudI.H
    MomentumCloud.C

\*---------------------------------------------------------------------------*/

#ifndef MomentumCloud_H
#define MomentumCloud_H

#include "particle.H"
#include "Cloud.H"
#include "timeIOdictionary.H"
#include "autoPtr.H"
#include "randomGenerator.H"
#include "standardNormal.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvMatrices.H"
#include "cloudSolution.H"
#include "fluidThermo.H"

#include "ParticleForceList.H"
#include "CloudFunctionObjectList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

class integrationScheme;

template<class CloudType>
class InjectionModelList;

template<class CloudType>
class DispersionModel;

template<class CloudType>
class PatchInteractionModel;

template<class CloudType>
class SurfaceFilmModel;

template<class CloudType>
class StochasticCollisionModel;


/*---------------------------------------------------------------------------*\
                      Class MomentumCloudName Declaration
\*---------------------------------------------------------------------------*/

TemplateName(MomentumCloud);


/*---------------------------------------------------------------------------*\
                        Class MomentumCloud Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class MomentumCloud
:
    public CloudType,
    public MomentumCloudName
{
public:

    // Public Typedefs

        //- Type of cloud this cloud was instantiated for
        typedef CloudType cloudType;

        //- Type of parcel the cloud was instantiated for
        typedef typename CloudType::particleType parcelType;

        //- Convenience typedef for this cloud type
        typedef MomentumCloud<CloudType> momentumCloudType;

        //- Force models type
        typedef ParticleForceList<MomentumCloud<CloudType>> forceType;

        //- Function object type
        typedef CloudFunctionObjectList<MomentumCloud<CloudType>>
            functionType;


private:

    // Private Data

        //- Cloud copy pointer
        autoPtr<MomentumCloud<CloudType>> cloudCopyPtr_;


protected:

    // Protected data

        //- Dictionary of particle properties
        IOdictionary particleProperties_;

        //- Dictionary of output properties
        timeIOdictionary outputProperties_;

        //- Solution properties
        cloudSolution solution_;

        //- Parcel constant properties
        typename parcelType::constantProperties constProps_;

        //- Sub-models dictionary
        const dictionary subModelProperties_;

        //- Switch to enable per-cell CPU load caching for load-balancing
        bool cpuLoad_;

        //- Random number generator - used by some injection routines
        mutable randomGenerator rndGen_;

        //- Standard normal distribution
        mutable distributions::standardNormal stdNormal_;

        //- Cell occupancy information for each parcel, (demand driven)
        autoPtr<List<DynamicList<parcelType*>>> cellOccupancyPtr_;

        //- Cell length scale
        scalarField cellLengthScale_;


        // References to the carrier gas fields

            //- Density [kg/m^3]
            const volScalarField& rho_;

            //- Velocity [m/s]
            const volVectorField& U_;

            //- Dynamic viscosity [Pa.s]
            const volScalarField& mu_;


        // Environmental properties

            //- Gravity
            const dimensionedVector& g_;

            //- Averaged ambient domain pressure
            scalar pAmbient_;


        //- Optional particle forces
        forceType forces_;

        //- Optional cloud function objects
        functionType functions_;


        // References to the cloud sub-models

            //- Injector models
            InjectionModelList<MomentumCloud<CloudType>> injectors_;

            //- Dispersion model
            autoPtr<DispersionModel<MomentumCloud<CloudType>>>
                dispersionModel_;

            //- Patch interaction model
            autoPtr<PatchInteractionModel<MomentumCloud<CloudType>>>
                patchInteractionModel_;

            //- Stochastic collision model
            autoPtr<StochasticCollisionModel<MomentumCloud<CloudType>>>
                stochasticCollisionModel_;

            //- Optional film model
            autoPtr<SurfaceFilmModel<MomentumCloud<CloudType>>> filmModel_;


        // Reference to the particle integration schemes

            //- Velocity integration
            autoPtr<integrationScheme> UIntegrator_;


        // Sources

            //- Momentum
            autoPtr<volVectorField::Internal> UTrans_;

            //- Coefficient for carrier phase U equation
            autoPtr<volScalarField::Internal> UCoeff_;


        // Initialisation

            //- Set cloud sub-models
            void setModels();


        // Cloud evolution functions

            //- Solve the cloud - calls all evolution functions
            template<class TrackCloudType>
            void solve
            (
                TrackCloudType& cloud,
                typename parcelType::trackingData& td
            );

            //- Build the cellOccupancy
            void buildCellOccupancy();

            //- Update (i.e. build) the cellOccupancy if it has
            //  already been used
            void updateCellOccupancy();

            //- Post-evolve
            void postEvolve();

            //- Reset state of cloud
            void cloudReset(MomentumCloud<CloudType>& c);


public:

    // Constructors

        //- Construct given carrier fields
        MomentumCloud
        (
            const word& cloudName,
            const volScalarField& rho,
            const volVectorField& U,
            const volScalarField& mu,
            const dimensionedVector& g,
            const bool readFields = true
        );

        //- Construct given carrier fields and thermo
        MomentumCloud
        (
            const word& cloudName,
            const volScalarField& rho,
            const volVectorField& U,
            const dimensionedVector& g,
            const fluidThermo& carrierThermo,
            const bool readFields = true
        );

        //- Copy constructor with new name
        MomentumCloud
        (
            MomentumCloud<CloudType>& c,
            const word& name
        );

        //- Copy constructor with new name - creates bare cloud
        MomentumCloud
        (
            const fvMesh& mesh,
            const word& name,
            const MomentumCloud<CloudType>& c
        );

        //- Disallow default bitwise copy construction
        MomentumCloud(const MomentumCloud&) = delete;

        //- Construct and return clone based on (this) with new name
        virtual autoPtr<lagrangian::Cloud<parcelType>> clone(const word& name)
        {
            return autoPtr<lagrangian::Cloud<parcelType>>
            (
                new MomentumCloud(*this, name)
            );
        }

        //- Construct and return bare clone based on (this) with new name
        virtual autoPtr<lagrangian::Cloud<parcelType>> cloneBare
        (
            const word& name
        ) const
        {
            return autoPtr<lagrangian::Cloud<parcelType>>
            (
                new MomentumCloud(this->mesh(), name, *this)
            );
        }


    //- Destructor
    virtual ~MomentumCloud();


    // Member Functions

        // Access

            //- Return a reference to the cloud copy
            inline const MomentumCloud& cloudCopy() const;


            // References to the mesh and databases

                //- Return particle properties dictionary
                inline const IOdictionary& particleProperties() const;

                //- Return output properties dictionary
                inline const IOdictionary& outputProperties() const;

                //- Return non-const access to the output properties dictionary
                inline IOdictionary& outputProperties();

                //- Return const access to the solution properties
                inline const cloudSolution& solution() const;

                //- Return access to the solution properties
                inline cloudSolution& solution();

                //- Return the constant properties
                inline const typename parcelType::constantProperties&
                    constProps() const;

                //- Return access to the constant properties
                inline typename parcelType::constantProperties& constProps();

                    //- Return reference to the sub-models dictionary
                inline const dictionary& subModelProperties() const;


            // Cloud data

                //- Return reference to the random generator
                inline randomGenerator& rndGen() const;

                //- Return reference to the standard normal distribution
                inline distributions::standardNormal& stdNormal() const;

                //- Return the cell occupancy information for each
                //  parcel, non-const access, the caller is
                //  responsible for updating it for its own purposes
                //  if particles are removed or created.
                inline List<DynamicList<parcelType*>>& cellOccupancy();

                //- Return the cell length scale
                inline const scalarField& cellLengthScale() const;


            // References to the carrier gas fields

                //- Return carrier gas velocity
                inline const volVectorField& U() const;

                //- Return carrier gas density
                inline const volScalarField& rho() const;

                //- Return carrier gas dynamic viscosity
                inline const volScalarField& mu() const;


            // Environmental properties

                //- Gravity
                inline const dimensionedVector& g() const;

                //- Return const-access to the ambient pressure
                inline scalar pAmbient() const;

                //- Return reference to the ambient pressure
                inline scalar& pAmbient();


            //- Optional particle forces
            inline const forceType& forces() const;

            //- Return the optional particle forces
            inline forceType& forces();

            //- Optional cloud function objects
            inline functionType& functions();


            // Sub-models

                //- Return const access to the injection model
                inline const InjectionModelList<MomentumCloud<CloudType>>&
                    injectors() const;

                //- Return reference to the injection model
                inline InjectionModelList<MomentumCloud<CloudType>>&
                    injectors();

                //- Return const-access to the dispersion model
                inline const DispersionModel<MomentumCloud<CloudType>>&
                    dispersion() const;

                //- Return reference to the dispersion model
                inline DispersionModel<MomentumCloud<CloudType>>&
                    dispersion();

                //- Return const-access to the patch interaction model
                inline const PatchInteractionModel<MomentumCloud<CloudType>>&
                    patchInteraction() const;

                //- Return reference to the patch interaction model
                inline PatchInteractionModel<MomentumCloud<CloudType>>&
                    patchInteraction();

                //- Return const-access to the stochastic collision model
                inline const
                    StochasticCollisionModel<MomentumCloud<CloudType>>&
                    stochasticCollision() const;

                //- Return reference to the stochastic collision model
                inline StochasticCollisionModel<MomentumCloud<CloudType>>&
                    stochasticCollision();

                //- Return const-access to the surface film model
                inline const SurfaceFilmModel<MomentumCloud<CloudType>>&
                    surfaceFilm() const;

                //- Return reference to the surface film model
                inline SurfaceFilmModel<MomentumCloud<CloudType>>&
                    surfaceFilm();


            // Integration schemes

                //-Return reference to velocity integration
                inline const integrationScheme& UIntegrator() const;


            // Sources

                // Momentum

                    //- Return momentum source
                    inline tmp<volVectorField::Internal> UTrans() const;

                    //- Access momentum source
                    inline volVectorField::Internal& UTransRef();

                    //- Return coefficient for carrier phase U equation
                    inline tmp<volScalarField::Internal> UCoeff() const;

                    //- Access coefficient for carrier phase U equation
                    inline volScalarField::Internal& UCoeffRef();

                    //- Return tmp momentum source term
                    inline tmp<fvVectorMatrix> SU
                    (
                        const volVectorField& U
                    ) const;


        // Check

            //- Total number of parcels
            inline label nParcels() const;

            //- Total mass in system
            inline scalar massInSystem() const;

            //- Total linear momentum of the system
            inline vector linearMomentumOfSystem() const;

            //- Total linear kinetic energy in the system
            inline scalar linearKineticEnergyOfSystem() const;

            //- Total rotational kinetic energy in the system
            inline scalar rotationalKineticEnergyOfSystem() const;

            //- Mean diameter Dij
            inline scalar Dij(const label i, const label j) const;

            //- Min diameter
            inline scalar Dmin() const;

            //- Max diameter
            inline scalar Dmax() const;


            // Fields

                //- Volume swept rate of parcels per cell
                inline const tmp<volScalarField> vDotSweep() const;

                //- Return the particle volume fraction field
                //  Note: for particles belonging to this cloud only
                inline const tmp<volScalarField> alpha() const;

                //- Return the particle mass fraction field
                //  Note: for particles belonging to this cloud only
                inline const tmp<volScalarField> Y() const;

                //- Return the particle effective density field
                //  Note: for particles belonging to this cloud only
                inline const tmp<volScalarField> rhoEff() const;


        // Cloud evolution functions

            //- Return true to cache per-cell CPU load
            bool cpuLoad() const
            {
                return cpuLoad_;
            }

            //- Set parcel thermo properties
            void setParcelThermoProperties(parcelType& parcel);

            //- Check parcel properties
            void checkParcelProperties
            (
                parcelType& parcel,
                const label injectori
            );

            //- Store the current cloud state
            void storeState();

            //- Reset the current cloud to the previously stored state
            void restoreState();

            //- Reset the cloud source terms
            void resetSourceTerms();

            //- Relax field
            template<class Type>
            void relax
            (
                DimensionedField<Type, volMesh>& field,
                const DimensionedField<Type, volMesh>& field0,
                const word& name
            ) const;

            //- Scale field
            template<class Type>
            void scale
            (
                DimensionedField<Type, volMesh>& field,
                const word& name
            ) const;

            //- Apply relaxation to (steady state) cloud sources
            void relaxSources(const MomentumCloud<CloudType>& cloudOldTime);

            //- Apply scaling to (transient) cloud sources
            void scaleSources();

            //- Pre-evolve
            void preEvolve();

            //- Evolve the cloud
            void evolve();

            //- Particle motion
            template<class TrackCloudType>
            void motion
            (
                TrackCloudType& cloud,
                typename parcelType::trackingData& td
            );

            //- Calculate the patch normal and velocity to interact with,
            //  accounting for patch motion if required.
            void patchData
            (
                const parcelType& p,
                const polyPatch& pp,
                vector& normal,
                vector& Up
            ) const;


        // Mapping

            //- Update topology using the given map
            virtual void topoChange(const polyTopoChangeMap&);

            //- Update from another mesh using the given map
            virtual void mapMesh(const polyMeshMap&);

            //- Redistribute or update using the given distribution map
            virtual void distribute(const polyDistributionMap&);


        // I-O

            //- Print cloud information
            void info();


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const MomentumCloud&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "MomentumCloudI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "MomentumCloud.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
