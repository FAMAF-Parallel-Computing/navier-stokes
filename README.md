# navier-stokes

$$
\nabla \cdot u = 0
$$

$$
\rho \frac{du}{dt} = - \nabla p + \mu \nabla^2u + F
$$

We could solve the diffusion directly as

$$
x_{t + 1} = x_t + a * \nabla^2 x_t
$$

where the laplacian operator is a 5 point stencil

$$
\nabla^2 x_{i,j} = x_{i, j - 1} + x_{i, j + 1} + x_{i - 1, j} + x_{i + 1, j} - 4 x_{i,j}
$$

but this explicit method is prone to numerical instability—especially when the time step or the diffusion coefficient is large. Small errors can rapidly accumulate, causing the simulation to become non-physical.

An alternative is to use an implicit method:

$$
x_{t + 1} - a * \nabla^2 x_{t + 1} = x_t
$$

Rearraging the terms we have

$$
(1 + 4a)x^{t + 1}_{i, j} - a \left(x^{t + 1}_{i, j - 1}\right)
$$

Then we proceed to solve the system

$$
(I - aA)x_{t + 1} = x_t
$$

Where

$$
A_{i, j} =
\begin{cases}
-4, & \text{if } p = q \quad \text{(the cell itself)} \\
1,  & \text{if } q \text{ is a direct neighbor of } p \\
0,  & \text{otherwise}
\end{cases}
$$

Which is a linear system that we can solve using
Gauss Seidel, nonetheless as the system is sparse
we can solve it directly

$$
x^{t + 1}_{i, j} = \frac{a(x^{t + 1}_{i, j - 1} + x^{t + 1}_{i, j + 1} + x^{t + 1}_{i - 1, j} + x^{t + 1}_{i + 1, j}) + x^t}{(1 + 4a)}
$$

## Bibliography

> Stam, J. (2003, March). Real-time fluid dynamics for games.
> In Proceedings of the game developer conference (Vol. 18, No. 11).
