# TechTheTown-LowLevel
Au boulot les 1As !

## TODO

- [x] Communication Ethernet de base
- [x] Ordres Des Actionneurs
- [ ] Asservissement
- [x] Capteurs
- [x] Hooks
- [x] Canal de Position (établir la fréquence d'envoie et le header associé avec le HL) & détection de fin de mouvement
- [x] Détection des blocages physiques et transmission sur le canal Event
- [ ] Mise en place de la basic Detection (arrêt si obstacle trop proche)
- [ ] Fusion de la gestion Série/Ethernet
- [ ] Gérer les ordres grâce à une classe pour que ce soit propre
- [ ] Mise en place de cas d'erreur explicites et plus nombreux pour le HL
- [ ] MotionControl point à point(avec réorientation progressive)
- [ ] Ordre propre pour la récupération de données d'asservissement + scripts en conséquence


## TABLE DES ORDRES
### ORDRES HL ⇒ LL
#### ORDRES GENERAUX

|   Ordres  |                       Actions                      |
|:---------:|:--------------------------------------------------:|
|     ?     |                     Ping le LL                     |
|    sus    |                    Switch les US                   |
|     f     |                 Check le mouvement                 |
|    ?xyo   |               Position + Orientation               |
|     d     |                  Translate de x mm                 |
|     t     |                   Tourne de α rad                  |
|    stop   |                        Stop                        |
|     cx    |                   Set x d'origine                  |
|     cy    |                   Set y d'origine                  |
|     co    |                   Set α d'origine                  |
|    cxyo   |                 Set x,y,α d'origine                |
|    ctv    |                Set translation speed               |
|    crv    |                 Set rotation speed                 |
|    ctrv   |           Set translation+rotation speed           |
|    efm    |                Enable forced movment               |
|    dfm    |               Disable forced movment               |
|    ct0    |          Désactive l'asserv en translation         |
|    ct1    |           Active l'asserv en translation           |
|    cr0    |           Désactive l'asserv en rotation           |
|    cr1    |             Active l'asserv en rotation            |
|    cv0    |            Désactive l'asserv en vitesse           |
|    cv1    |             Active l'asserv en vitesse             |
|   seti2c  |                Set les capteurs I2C                |
|    cod    |            Retourne les ticks de codeuse           |
|  pfdebug  |            Info de debug sur la position           |
|   rawpwm  |        Demande un PWM brut aux deux moteurs        |
|   getpwn  |          Retourne le PWN des deux moteurs          |
|   errors  |         Retourne les erreurs d'incertitude         |
|  rawspeed |               Vitesse brute des roues              |
| rawposdata|         Pos x,y,α; vL,vR, targetvL,targetvR        |
| monthlery |                Mode de présentation                |
|     av    |                       Avance                       |
|     rc    |                       Recule                       |
|     td    |                   Tourne à droite                  |
|     tg    |                   Tourne à gauche                  |
|   sstop   |                        Arrêt                       |
|     nh    | Créé un nouveau hook (id,x,y,r,α,tolerance,action) |
|     eh    |                   Active le hook                   |
|     dh    |                  Désactive le hook                 |


#### ORDRES DE CONTRÔLE D'ACTION

|   Ordres  |                       Actions                      |
|:---------:|:--------------------------------------------------:|
|    AXm    |              Envoie l'AX12 à un α en °             |
|    AXGm   |             Envoi un groupe à un α en °            |
|    AXs    |            Modifie la vitesse d'un AX12            |
|    AXGs   |           Modifie la vitesse d'un groupe           |
|    alp    |                  Active la pompe                   |
|    dlp    |                 Désactive la pompe                 |
|  blbAbei  |         Baisse le bras avant pour abeille          |
|   blbAv   |                Baisse le bras avant                |
|   rlbAv   |                Relève le bras avant                |
|   flpAv   |                Ferme la porte avant                |
|   olpAv   |                Ouvre la porte avant                |
|   tlpAv   |                Tilt la porte avant                 |
|    aeAv   |            Active l'électrovanne avant             |
|    deAv   |           Désactive l'électrovanne avant           |
|   blbAr   |               Baisse le bras arrière               |
|   rlbAr   |               Relève le bras arrière               |
|   flpAr   |               Ferme la porte arrière               |
|   olpAr   |               Ouvre la porte arrière               |
|   tlpAr   |               Tilt la porte arrière                |
|    aeAr   |           Active l'électrovanne arrière            |
|    deAr   |          Désactive l'électrovanne arrière          |


### ORDRES SPECIFIQUES LL

|   Ordres  |                       Actions                      |
|:---------:|:--------------------------------------------------:|
|   toggle  |         Change le mode de réglage d'asserv         |
|  display  |          Retourne les constantes d'asserv          |
|    kpt    |              Set le kp de translation              |
|    kdt    |              Set le kd de translation              |
|    kit    |              Set le ki de translation              |
|    kpr    |                Set le kp de rotation               |
|    kdt    |                Set le kd de rotation               |
|    kit    |                Set le ki de rotation               |
|    kpg    |            Set le kp de vitesse à gauche           |
|    kdg    |            Set le kd de vitesse à gauche           |
|    kig    |            Set le ki de vitesse à gauche           |
|    kpd    |            Set le kd de vitesse à droite           |
|    kdg    |            Set le kd de vitesse à droite           |
|    kig    |            Set le ki de vitesse à droite           |
