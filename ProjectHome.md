This software provides a mathematical tool using Conley Index theory and Morse decompositions to describe dynamical systems. The main principle is described in

> Arai, Zin, William Kalies, Hiroshi Kokubu, Konstantin Mischaikow, Hiroe Oka, and Pawel Pilarczyk. "A database schema for the analysis of global dynamics of multiparameter systems." SIAM Journal on Applied Dynamical Systems 8, no. 3 (2009): 757-789.

More recently it has been described in

> Bush, Justin, Marcio Gameiro, Shaun Harker, Hiroshi Kokubu, Konstantin Mischaikow, Ippei Obayashi, and Paweł Pilarczyk. "Combinatorial-topological framework for the analysis of global dynamics." Chaos: An Interdisciplinary Journal of Nonlinear Science 22, no. 4 (2012): 047508.

The software is written in C++ and uses the Boost libraries. Additionally, it uses the SDSL (Succinct Data Structure Library), see

> Gog, Simon, Timo Beller, Alistair Moffat, and Matthias Petri. "From Theory to Practice: Plug and Play with Succinct Data Structures." arXiv preprint arXiv:1311.1249 (2013).

For homology computations, we make use of CHomP, which is included. These homology algorithms rely on discrete Morse theory algorithms. See

> Harker, Shaun, Konstantin Mischaikow, Marian Mrozek, and Vidit Nanda. "Discrete Morse theoretic algorithms for computing homology of complexes and maps." Foundations of Computational Mathematics 14, no. 1 (2014): 151-184.

To characterize Conley Index, we use field coefficients and must find the Frobenius Normal form of an induced map on homology. CHomP provides the induced map on homology. To find the Frobenius Normal form we use the algorithm of Storjohann:

> Storjohann, Arne. "An O (n 3) algorithm for the Frobenius normal form." In Proceedings of the 1998 international symposium on Symbolic and algebraic computation, pp. 101-105. ACM, 1998.
