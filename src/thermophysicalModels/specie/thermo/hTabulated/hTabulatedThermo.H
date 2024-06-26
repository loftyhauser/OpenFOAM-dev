/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2020-2023 OpenFOAM Foundation
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
    Foam::hTabulatedThermo

Description
    Enthalpy based thermodynamics package using uniform tabulated data for
    enthalpy and heat capacity vs pressure and temperature.

Usage

    \table
        Property     | Description
        hf           | Heat of formation
        sf           | Standard entropy
        hs           | Sensible enthalpy vs pressure and temperature table
        Cp           | Specific heat capacity vs pressure and temperature table
    \endtable

    Example of the specification of the thermodynamic properties:
    \verbatim
    thermodynamics
    {
        hf              0;
        sf              0;

        hs
        {
            pLow    1e4;
            pHigh   5e5;

            Tlow    200;
            Thigh   1500;

            values
            <m> <n>
            (
                (..........)
                .
                .
                .
                (..........)
            );
        }

        Cp
        {
            pLow    1e3;
            pHigh   1e6;

            Tlow    200;
            Thigh   1500;

            values
            <m> <n>
            (
                (..........)
                .
                .
                .
                (..........)
            );
        }
    }
    \endverbatim

SourceFiles
    hTabulatedThermoI.H
    hTabulatedThermo.C

See also
    Foam::Function2s::uniformTable

\*---------------------------------------------------------------------------*/

#ifndef hTabulatedThermo_H
#define hTabulatedThermo_H

#include "UniformTable2.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class EquationOfState>
class hTabulatedThermo;

template<class EquationOfState>
Ostream& operator<<
(
    Ostream&,
    const hTabulatedThermo<EquationOfState>&
);


/*---------------------------------------------------------------------------*\
                      Class hTabulatedThermo Declaration
\*---------------------------------------------------------------------------*/

template<class EquationOfState>
class hTabulatedThermo
:
    public EquationOfState
{
    // Private Typedefs

        //- Table type
        typedef Function2s::UniformTable<scalar> table2D;


    // Private Data

        //- Heat of formation
        scalar hf_;

        //- Standard entropy
        scalar sf_;

        //- Sensible enthalpy table [J/kg]
        table2D hs_;

        //- Specific heat at constant pressure table [J/kg/K]
        table2D Cp_;

        //- Specific heat at constant volume table [J/kg/K]
        table2D Cv_;


public:

    // Constructors

        //- Construct from name and dictionary
        hTabulatedThermo(const word& name, const dictionary& dict);

        //- Construct as a named copy
        inline hTabulatedThermo(const word&, const hTabulatedThermo&);


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "hTabulated<" + EquationOfState::typeName() + '>';
        }

        //- Limit the temperature to be in the range Tlow_ to Thigh_
        inline scalar limit(const scalar) const;


        // Fundamental properties

            //- Heat capacity at constant pressure [J/kg/K]
            inline scalar Cp(const scalar p, const scalar T) const;

            //- Heat capacity at constant volume [J/kg/K]
            inline scalar Cv(const scalar p, const scalar T) const;

            //- Sensible enthalpy [J/kg]
            inline scalar hs(const scalar p, const scalar T) const;

            //- Absolute enthalpy [J/kg]
            inline scalar ha(const scalar p, const scalar T) const;

            //- Sensible internal energy [J/kg]
            inline scalar es(const scalar p, const scalar T) const;

            //- Absolute internal energy [J/kg]
            inline scalar ea(const scalar p, const scalar T) const;

            //- Enthalpy of formation [J/kg]
            inline scalar hf() const;

            //- Entropy [J/kg/K]
            inline scalar s(const scalar p, const scalar T) const;

            //- Gibbs free energy of the mixture in the standard state [J/kg]
            inline scalar gStd(const scalar T) const;


        // Derivative term used for Jacobian

            //- Temperature derivative of heat capacity at constant pressure
            inline scalar dCpdT(const scalar p, const scalar T) const;


        // I-O

            //- Write to Ostream
            void write(Ostream& os) const;


    // Ostream Operator

        friend Ostream& operator<< <EquationOfState>
        (
            Ostream&,
            const hTabulatedThermo&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "hTabulatedThermoI.H"

#ifdef NoRepository
    #include "hTabulatedThermo.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
