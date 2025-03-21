# navier-stokes

$$
\nabla \cdot u = 0
$$

$$
\rho \frac{du}{dt} = - \nabla p + \mu \nabla^2u + F
$$

We could solve the diffusion directly as

$$
x_{i + 1} = x_i + a * \nabla^2 x_i
$$

where the laplacian operator is a 4 point stencil

$$
\nabla^2 x_{i,j} = x_{i, j - 1} + x_{i, j + 1} + x_{i - 1, j} + x_{i + 1, j} - 4 x_{i,}
$$

but this explicit method is prone to numerical instability—especially when the time step or the diffusion coefficient is large. Small errors can rapidly accumulate, causing the simulation to become non-physical.

An alternative is to use an implicit method:

$$
x_{i + 1} - a * \nabla^2 x_{i + 1} = x_i
$$

Rearraging the terms we have

$$
(1 + 4a)x_{i, j} - a( x_{i, j - 1} + x_{i, j + 1} + x_{i - 1, j} + x_{i + 1, j})
$$

Then we proceed to solve the system

$$
(I - aA)x_{i + 1} = x_i
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

## Bibliography

> Stam, J. (2003, March). Real-time fluid dynamics for games.
> In Proceedings of the game developer conference (Vol. 18, No. 11).
