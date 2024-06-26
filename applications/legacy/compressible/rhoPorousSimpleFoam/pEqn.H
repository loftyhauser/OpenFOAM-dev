{
    const volScalarField& psi = thermo.psi();

    tmp<volVectorField> tHbyA;
    if (pressureImplicitPorosity)
    {
        tHbyA = constrainHbyA(trTU()&UEqn.H(), U, p);
    }
    else
    {
        tHbyA = constrainHbyA(trAU()*UEqn.H(), U, p);
    }
    volVectorField& HbyA = tHbyA.ref();

    tUEqn.clear();

    bool closedVolume = false;

    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        fvc::interpolate(rho)*fvc::flux(HbyA)
    );
    MRF.makeRelative(fvc::interpolate(rho), phiHbyA);

    closedVolume = adjustPhi(phiHbyA, U, p);

    while (simple.correctNonOrthogonal())
    {
        tmp<fvScalarMatrix> tpEqn;

        if (pressureImplicitPorosity)
        {
            tpEqn =
            (
                fvm::laplacian(rho*trTU(), p)
              + fvModels.sourceProxy(rho, p)
             ==
                fvc::div(phiHbyA)
            );
        }
        else
        {
            tpEqn =
            (
                fvm::laplacian(rho*trAU(), p)
              + fvModels.sourceProxy(rho, p)
             ==
                fvc::div(phiHbyA)
            );
        }

        fvScalarMatrix& pEqn = tpEqn.ref();

        pEqn.setReference
        (
            pressureReference.refCell(),
            pressureReference.refValue()
        );

        pEqn.solve();

        if (simple.finalNonOrthogonalIter())
        {
            phi = phiHbyA - pEqn.flux();
        }
    }

    #include "incompressible/continuityErrs.H"

    // Explicitly relax pressure for momentum corrector
    p.relax();

    if (pressureImplicitPorosity)
    {
        U = HbyA - (trTU() & fvc::grad(p));
    }
    else
    {
        U = HbyA - trAU()*fvc::grad(p);
    }

    U.correctBoundaryConditions();
    fvConstraints.constrain(U);

    fvConstraints.constrain(p);

    // For closed-volume cases adjust the pressure and density levels
    // to obey overall mass continuity
    if (closedVolume)
    {
        p += (initialMass - fvc::domainIntegrate(psi*p))
            /fvc::domainIntegrate(psi);
        p.correctBoundaryConditions();
    }

    rho = thermo.rho();
    rho.relax();
}
