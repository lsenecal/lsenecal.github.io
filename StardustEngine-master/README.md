# Stardust

_Ou génération procédurale de planètes_

Ce projet a pour objectif de pouvoir générer de façon
procédurale des planètes et les afficher en 3D à l'écran.
On s'attachera dans un premier temps à obtenir une
planète à partir d'un jeu de paramètre et pouvoir la
visualiser dans une fenêtre sous différents angles (en
utilisant la souris ou le clavier) en se limitant à des couleurs
simples. On se propose d'utiliser dans ce TER le langage
C++, l’API OpenGL et de manipuler les voxels.

Les amélioration possibles pourront se porter dans un
premier temps sur la qualité des textures utilisées,
l'implémentation d'un niveau de détail dynamique
permettant notamment de visualiser la planète depuis son
sol ainsi que l'ajout de détails tel qu'une végétation et une
eau dynamique (non plus une simple texture).

L'optimisation et le nombre d'image par seconde étant un
facteur crucial, on pourra s'intéresser à la parallélisation des
tâches du coté CPU d'une part (utilisation de threads ou
OpenMP) et coté GPU d'autre part (utilisation de CUDA et
de OpenGL Interoperability).

Pour finir, une couche physique pourra être ajouté afin de
gérer les collisions et la gravité. Ces ajouts permettront le
déplacement sur la planète de l'observateur. Il sera possible
de créer des planètes plus petites pour créer des lunes en
orbites autour de la planète.
