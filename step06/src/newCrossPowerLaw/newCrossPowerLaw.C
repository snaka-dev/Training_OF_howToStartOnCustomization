/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd.
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

#include "newCrossPowerLaw.H"
#include "addToRunTimeSelectionTable.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace viscosityModels
{
    defineTypeNameAndDebug(newCrossPowerLaw, 0);

    addToRunTimeSelectionTable
    (
        viscosityModel,
        newCrossPowerLaw,
        dictionary
    );
}
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::viscosityModels::newCrossPowerLaw::calcNu()
{
    strainRate_ = strainRate();
    return (nu0_ - nuInf_)/(scalar(1) + pow(m_*strainRate_, n_)) + nuInf_;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::viscosityModels::newCrossPowerLaw::newCrossPowerLaw
(
    const word& name,
    const dictionary& viscosityProperties,
    const volVectorField& U,
    const surfaceScalarField& phi
)
:
    viscosityModel(name, viscosityProperties, U, phi),
    newCrossPowerLawCoeffs_
    (
        viscosityProperties.optionalSubDict(typeName + "Coeffs")
    ),
    nu0_("nu0", dimViscosity, newCrossPowerLawCoeffs_),
    nuInf_("nuInf", dimViscosity, newCrossPowerLawCoeffs_),
    m_("m", dimTime, newCrossPowerLawCoeffs_),
    n_("n", dimless, newCrossPowerLawCoeffs_),
    strainRate_
    (
        IOobject
        (
            "strainRate",
            U_.time().timeName(),
            U_.db(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        U_.mesh(),
        dimensionedScalar("strainRate", dimVelocity/dimLength, Zero) 
    ),
    nu_
    (
        IOobject
        (
            name,
            U_.time().timeName(),
            U_.db(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        calcNu()
    )
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::viscosityModels::newCrossPowerLaw::read
(
    const dictionary& viscosityProperties
)
{
    viscosityModel::read(viscosityProperties);

    newCrossPowerLawCoeffs_ =
        viscosityProperties.optionalSubDict(typeName + "Coeffs");

    newCrossPowerLawCoeffs_.readEntry("nu0", nu0_);
    newCrossPowerLawCoeffs_.readEntry("nuInf", nuInf_);
    newCrossPowerLawCoeffs_.readEntry("m", m_);
    newCrossPowerLawCoeffs_.readEntry("n", n_);

    return true;
}


// ************************************************************************* //
