# -*- coding: UTF-8 -*-
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
# import _tempplot


def calAEI(N, a, Sz):
    S = 2 * np.sqrt(a) - (Sz - N)
    return getAEI(N, S, Sz)


def getAEI(N, S, Sz):
    L = (S + Sz - N) / 2
    e = (Sz - N - S) / (S + Sz - N)
    I = Sz / (L * e) - 1
    e = np.sqrt(1 - e * e)
    I = np.arccos(I) * 180 / np.pi
    a = L**2

    return a, e, 180 - I


mu = 0.001
mu_s = 1 - mu


LOCATION = "assets/old/1206/planar/"
index = 2


def NStoAE(N, S):
    B = S + N
    C = S - N
    a = (C / 2)**2
    e = np.sqrt(1 - (B / C)**2)
    return a, e


target = LOCATION + "SingleAve_-1.96_0.08.txt"
desktop = "~/Desktop/"
N = -1.96
S_min = 0
S_max = 0.08
# a = 1.05
# target = LOCATION + "Ast_" + str(index) + ".txt"
# target2 = LOCATION + "A_" + str(index) + ".txt"
# data2 = np.loadtxt(target2, skiprows=4)

# levels = np.linspace(-1.6, -1.4, 20)
# print levels

data = np.loadtxt(target)
# a, e = NStoAE(data[:, 0], data[:, 1])

n = 721
m = data.shape[0] / n
# print n, m
# x = np.linspace(0, 360, n)
x = np.linspace(-0, 360, n) / 180.0 * np.pi
y = np.linspace(S_min, S_max, m)
e = NStoAE(N, y)[1]
# y = calAEI(N, a, y)[2]
X, Y = np.meshgrid(x, e)
Z = data
Z = Z.reshape(m, n)
Z = np.nan_to_num(Z)
Zmax = np.max(Z)
Zmin = np.min(Z)
level = np.linspace(Zmin, Zmin + 0.0011, 10)
level = np.insert(level, 1, Zmin + 0.00001)

print level

level2 = np.linspace(Zmin, Zmin + 0.0028, 100)
# level2 = np.hstack((level2, Zmin + 0.0037))

# fig, ax = plt.subplots()
fig, ax = plt.subplots(subplot_kw=dict(projection='polar'))
# fig2, ax2 = plt.subplots()
ax.contour(X, Y, Z, level, colors='black', linewidths=0.7, alpha=0.25)
ax.contourf(X, Y, Z, level2, cmap='magma_r')
ax.grid(color='black', linestyle='dotted', alpha=0.2, linewidth=1)
ax.grid(False)
# ax.xaxis.grid(True)

# ax.contour(X, Y, Z, level2, colors='k')
ax.set_xlim(0, 360)
# ax.set_ylim(0.7, 0.8)
# cbar = plt.colorbar(CS)x
fig.savefig("N=-1.96.png", transparent=True, dpi=500)
plt.show()
