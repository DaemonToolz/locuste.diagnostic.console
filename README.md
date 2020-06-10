# locuste.system.connector
LOCUSTE : Connecteur console pour des interactions directes avec les modules (Golang / Python)

<img width="2609" alt="locuste-diagnostics-banner" src="https://user-images.githubusercontent.com/6602774/84285960-5c1dcb80-ab3e-11ea-9166-373c2500a8f2.png">

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/3cb9cb73d93843ecb62cd2951c60d37d)](https://www.codacy.com/manual/axel.maciejewski/locuste.system.connector?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DaemonToolz/locuste.system.connector&amp;utm_campaign=Badge_Grade)

Le project Locuste se divise en 4 grandes sections : 
* Automate (Drone Automata) PYTHON (https://github.com/DaemonToolz/locuste.drone.automata)
* Unité de contrôle (Brain) GOLANG (https://github.com/DaemonToolz/locuste.service.brain)
* Unité de planification de vol / Ordonanceur (Scheduler) GOLANG (https://github.com/DaemonToolz/locuste.service.osm)
* Interface graphique (UI) ANGULAR (https://github.com/DaemonToolz/locuste.dashboard.ui)

![Composants](https://user-images.githubusercontent.com/6602774/83644711-dcc65000-a5b1-11ea-8661-977931bb6a9c.png)

Tout le système est embarqué sur une carte Raspberry PI 4B+, Raspbian BUSTER.
* Golang 1.11.2
* Angular 9
* Python 3.7
* Dépendance forte avec la SDK OLYMPE PARROT : (https://developer.parrot.com/docs/olympe/, https://github.com/Parrot-Developers/olympe)


![Vue globale](https://user-images.githubusercontent.com/6602774/83644783-f10a4d00-a5b1-11ea-8fed-80c3b76f1b00.png)

Détail des choix techniques pour la partie Outil de diagnostique :

* [C++] - L'intégration C/C++ permet d'avoir un contrôle accru sur les performances de l'application, bien que le Go n'utilise qu'une infime fraction de la RAM, le C++ reste néanmoins optimal. Chaque application utilisera 2 pipes : une d'entrée, une de sortie. 
* [Named Pipe] - Utilisation des named pipe est une des façons les plus pratiques et rapide pour communique entre 2 processus

![diagnostic_unit](https://user-images.githubusercontent.com/6602774/83643505-61b06a00-a5b0-11ea-8d57-879fcb9cd6fc.png)

Evolutions à venir : 
* Stabilité de l'outil logiciel 
* Plus de fonctionnalités (commandes / menus d'aides / démarrage et arrêt d'applications en local)
