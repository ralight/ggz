	<h1>
		<span class="itemleader">:: </span>
GGZ Community Blogs		<span class="itemleader"> :: </span>
		<a name="planet"></a>
	</h1>


	<h2>
		<span class="itemleader">:: </span>
December 06, 2004		<span class="itemleader"> :: </span>
		<a name="planetdate"></a>
	</h2>

	<div class="text">

<a href="http://www.ggzgamingzone.org/~josef/" title="Josef Spillner">
josef</a>

:
<a href="http://www.ggzgamingzone.org/~josef/">
I'm blogging!</a>

	<br>

<a href="http://www.ggzgamingzone.org/~josef/">December 06, 2004 11:23 PM</a>


	</div>
	<div class="text">

For the sake of starting to fill some feed aggregators,
here will be my GGZ blog from now on.
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/16267.html">
Gaim 1.1.0 iz out</a>

	<br>

<a href="http://linuxfr.org/~phhusson/16267.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Gaim 1.1.0 est sorti hier
3 nouvelles fonctionnalités

-Il peut chercher ses librairie ailleur que ce qui a ete donné a la compil par --prefix

-Nouveau protocol MSN pour les icones
ET
le plus important tadam:
-Meilleur support des différents charset :)

et différentes corrections de bugs

PS:Je trouve que l'auteur fait des changements de versions "majeur" "abusivement", mais bon c'est vrai que pour rendre gaim plus populaire aupres des windowsiens faut faire croire que.....
bon 

PPS:oula ca passe bizarrement les superieur et inferieurs en visualisations, j'espere que ca passera bien en final
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/16263.html">
Knoda, remplacant de kexi avant l'heure</a>

	<br>

<a href="http://linuxfr.org/~phhusson/16263.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Tout le monde attend kexi, l'access-like pour KDE, depuis longtemps, et on désesper(ait?) de voir quelque chose arriver, et voila qu'un petit frangin arrive: knoda.
Il peut faire tout ce qu'on peut demander à kexi (enfin j'ai pas sa liste des features de kexi sous la main):
-créer et supprimer des base de données (encore heureux)
-créer et supprimer des tables
-executer des requetes SQL normales
-import et export en CSV 
-Utilisation de formulaires (c'est un peu le but non?)
-Support de mysql, Postgresql , SQLite et ODBC
Et le meilleur pour la fin
On peut faire des plugins en python soit meme :)
je ne vous fais pas attendre plus longtemps pour l'url:
http://www.knoda.org/(...)

PS:bon apres avoir fait ses eloges me reste plus qu'a le tester :)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/16257.html">
Usb Storage</a>

	<br>

<a href="http://linuxfr.org/~phhusson/16257.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Pour ceux qui n'auraient plus de /dev/sdX avec des disques amovibles USB mais avec un kernel récent, ne vous inquietez pas, il semblerait que les périf usb se soient vu privé d'emulation scsi et sont donc maintenant directement utilisable en devfs ca donne: /dev/ub/a/part0 pour l'ex sda1
ou a est le numero du perif amovible et part n'a pas besoin d'etre expliqué
(je suis en 2.6.10-rc1)
dans /proc/devices j'ai:
125 ub
voila voila c'est tout pour aujourd'hui :)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/16230.html">
Asprintf</a>

	<br>

<a href="http://linuxfr.org/~phhusson/16230.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Cher journal je vais te faire part d'une decouvèrte que j'ai faite et qui apparement n'est pas tres connu mais justement meriterait de l'être:
asprintf
Cette fonction est l'équivalent de sprintf, mais elle alloue à la taille voulue le buffer passé par un pointeur en premier argument.
Seul probleme c'est que pour la glibc donc adieu portabilité.
Dans le meme genre le %a de sscanf alloue automatiquement la mémoire nécessaire
Bon voila si ca pouvait en aider quelques uns
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/15007.html">
Gaim-ggz (ou comment occuper ses vacances avec presque aucune connection internet)</a>

	<br>

<a href="http://linuxfr.org/~phhusson/15007.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Y a une semaine je postais pour trouver comment faire un plugin pour gaim pour pouvoir utiliser GGZ Gaming Zone sur son client IM favoris (bon j'utilises pas vraiment d'IM personnelement mais bon voila quoi ;)
Et pendant une semaine sans connection d'ordinateurs j'ai pondu ca:
http://phhusson.free.fr/gaim-ggz.tgz(...)
(c'est crade et tout ce qu'on veut
mais ca marche)
Pour le configurer/le compiler/l'installer on fait tout simplement:
cd ggz-test
vim test-ggz.c(un troll? meuh nan un reflexe)
a la place de char *server="127.0.0.1";
mettez le nom (ou l'ip) de vore server GGZ (avec une seule machine pour moi ca pouvait pas etre autre chose que 127.0.0.1 ;)
faites un petit make
allez dans gaim-plugin
editez simple.c
et remplacez le #define GGZ_PHH "/home/alh/ggz-test/test-ggz"
par l'emplacement du binaire obtenu lors du precedant make
apres faites un petit sh compile
et pour finir deux petits liens:
ln -s `pwd`/simple.la /usr/lib/gaim/simple.la
ln -s `pwd`/simple.lo /usr/lib/gaim/simple.so

Et ca doit etre tout
Si ca suffit pas postez votre meconcantement :)
Ah oui j'oublie le principal,
comment on utilise le bousin;
c'est "tres" simple dans votre fenetre d'observation vous clickez sur le bouton Jeu
Vous pouvez aussi utiliser la commande /jeu (les arguments sont donnés)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/14593.html">
"Nouveau" site pour directfb</a>

	<br>

<a href="http://linuxfr.org/~phhusson/14593.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Le site de directfb a changé de couleurs et de logo!!!!!
Le nouveau logo est plus "attrayant" et le menu a changé de goùt stout
bon voila
http://www.directfb.org(...)
PS:j'aimerais bien savoir ceux que directfb interesse savoir si je suis un des rare a m'y interesser ou pas
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/14588.html">
Driver nvidia 6XXX</a>

	<br>

<a href="http://linuxfr.org/~phhusson/14588.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Voila j'ai lu ces derniers temps que les derniers drivers nvidia etait tres lent par rapport aux precendents et personne n'a reussi a trouver pourquoi
du moins je n'ai rien vu qui parlait de ca
J'ai installé un nouveau linux sur mon lvm,
donc nouvelle installation autant prendre les derniers drivers
installation sans probleme comme toujours
maintenant je teste
arg ca rame on croirait de l'emulation software!!!! (j'exagere un peu mais n'empeche que c'etait inutiliable)
alors je ne me laisse pas abattre je vais voir dans mes options de configuration noyau si j'ai agpart ou pas (sur la version precedente ca m'avait enlevé un "gros" bug, j'ai du la marqué dans un de mes journaux)
Et la je m'apercois que j'avais compilé mon kernel sans agpgart
donc je recompil avec (ne pas oublié de cocher la case pour votre carte mere avec, chez moi c'etait nforce2)
je reboot
je teste
et ca marche parfaitement :)
(au detail pret que je me retrouve dans mon dmesg avec des:
NVRM: couldn't find alloc for 0x2f000070
NVRM: couldn't find alloc for 0x2f800070
mais la version precedente y avait des messages encore plus long et elle marchait)
sinon niveau acpi j'vais tenter mais j'ai deja du mal a le faire marcher en console donc bon pour le faire sous X.org ca va attendre un peu :)

NB:pratique la recherche automatique dans les autres journaux
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/14063.html">
Téléphonie en modem rtc</a>

	<br>

<a href="http://linuxfr.org/~phhusson/14063.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Linux est un OS sécurisé et performant sur la trame TCP/IP. Mais qu'en est-il de son exploitation des fonctions vocales d'un modem RTC ? Voila une question, qui à ma connaissance reste en suspend ...
Il serait fort appréciable en effet d'exploiter les fonctions possibles de telephonie, de répondeur, d'enregistreur que tout modem (RTC) sous-entend
Vu que Linux intégre désormais très bien la plupart des "anciens" modem RTC, ces fonctionnalité de téléphonie seraient pr conséquent fortement compatibilisées, à priori. On pourrait imaginer un enregistrement de conversation "à la volée" en MP3, ainsi que des messages vocaux. Et afin d'etre le plus complet possible dans le dommaine de l'exploitation telephonique, un lien pourrait etre fait avec xtel, emulateur de minitel pour linux.
De plus, quel bonheur de pouvoir intégrér des fct d'appel telephonique dans des appli comme korganiser ou autre carnet d'adresse, ainsi que de faire les liens correspondant à l'envoi et à la réception de fax (appli existante)
Enfin, à l'aide d'un GUI simple pouvoir accéder à ces fonctions via un simple clic !

NB:Je ne fais que le messager de quelqu'un d'autre; c'est pas moi qui ai fait ce texte la :)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/14007.html">
Merci les devels DLFP :)</a>

	<br>

<a href="http://linuxfr.org/~phhusson/14007.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Ah j'ai enfin cette toolbar dans konqueror!!!
Merci a ceux/celui qui a corriger ce problème!!
bon voila c'est tout ce que je veux dire
oh et pis:
Bonne nuit a ceux qui se couchent
Bonjour à ce qui se lèvent
et euh pour ceux qui restent éveiller ben euh
soit travaillez bien soit amusez vous bien
ça dépend de ce que vous faites ;)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/12827.html">
Sortie de geexbox 0.97</a>

	<br>

<a href="http://linuxfr.org/~phhusson/12827.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Ça y est
après un long moment d'inactivité (au niveau des releases)geexbox repars:
voici le changelog:
 * Passage à MPlayer 1.0pre4
 * Support des télécommandes et récépteurs Packard Bell Fast media.
 * Support des cartes DXR3/Hollywood+ (décodeurs MPEG1/2 matériel) (Sortie TV uniquement pour l'instant).
 * Support de la sortie TV des cartes S3 Virge GX2.
 * Support du conteneur audio/vidéo Matroska.
 * Support du conteneur Nullsoft streaming video (NSV).
 * Support des cartes réseaux WiFi aux formats PCI et USB (chipsets AiroNet, Atmel, Atheros, Prism2 et Prism54/GT).
 * Support des disques Serial ATA.
 * Support des cartes Ethernet 3COM/NetGear Gigabit.
 * Support des cartes BT8x8 et Saa7134 (entrées Composite et S-VHS et des tuners TV).
 * Support du streaming audio/vidéo (Web-Radios etc ...)
 * Accès à Internet via des passerelles web.
 * Ajout d'un serveur Telnet pour prise en main distante.
 * Support de l'acceleration vidéo matérielle (VidiX) pour les cartes EPIA-M (chipset CLE266) et ATI Radeon 9xxx.
 * Support VidiX pour les cartes nVidia (EXPERIMENTAL : désactivé par défaut).
 * Ajout de la librairie IrMan.
 * Support des télécommandes ATI Remote Wonder via LIRC (temps de réponse amélioré et configuration des touches possibles).
 * Ajout d'un visualiseur d'images (FBI) avec support des formats BMP, GIF, JPEG, PCD, PNG, PNM et PPM.
 * Passage du gestionnaire de version CVS à GNU Arch.

ah j'ai failli oublié, l'url:
http://www.geexbox.org/fr/index.html(...)

En espérant que la news/le journal est pas déjà passé.......
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/12564.html">
Nouvelle distribution: qu'en attendre?</a>

	<br>

<a href="http://linuxfr.org/~phhusson/12564.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Voila je suis entrain de développer une distribution,
avec un systeme de package quasi terminé en C et utilisant la libxml2 pour la base de donnée,
Pour l'instant le but de cette distribution est principalement pour le fun ;)
Seulement j'aimerais bien que mon travail ne soit pas seulement pour le fun,
ma distribution (oui bon pas MA, on la fait a deux avec http://linuxfr.org/~linuxcode/(...) vous savez l'adorateur d'ipv6 ;)) sera tout aussi utilisable avec des binaires qu'avec des sources,
seulement je me leure pas, ceci n'attirera que peu de monde,
et la majorité préférera nasgaia ou gentoo (bien que la gestion des binaires ne soit pas encore très simple apparement)
c'est pour ca que je pose cette question:
Qu'attendre de cette nouvelle distrib?
Faire des installations simples et rapides?
non merci mandrake essaye depuis longtemps sans trop de succes
mais a par ca?
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/12355.html">
C'est quoi cette barre en bas?</a>

	<br>

<a href="http://linuxfr.org/~phhusson/12355.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

eh mais on peut changer la feuille de style, le seuil et lire le nombre de nouveau commentaires
bravo a l'&quipe pour cette avancée très importante ;)
PS:a quand un correcteur grammaticale?
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/11711.html">
Emuler un mac recent sur x86</a>

	<br>

<a href="http://linuxfr.org/~phhusson/11711.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

J'aimerais pouvoir emuler un mac os quelquonc (8,9,X) sur mon p'ti athlon,
apparement c'est infasable actuellement,
mais j'ai eu une idée (a mon avis pas si lumineuse que ca mais on vera bien):
Utiliser qemu pour demarer un linux ppc(jusque la ca va?)
puis mettre sur ce qemu un mac on linux?
est-ce faisable?
Actuellement j'ai pas ce qu'il faut pour tester mais si quelqu'un peux?
ou me dire que c'est infasable en disant comment?
ou me dire d'attendre que qemu supporte la virtualisation complete d'un mac :)
(NON pas de executer ou synppc)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/11454.html">
Driver 3D proprios: des questions, que de questions</a>

	<br>

<a href="http://linuxfr.org/~phhusson/11454.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Bon je commence directos sans intro (et pis quoi encore c'est pas une redac ou je ne sais trop quoi!!!):
Pourquoi ATI et NVIDIA produisent (c'est un bien grand mot) des drivers uniquement compatible avec XFree86?
Ils pouraient pas plutot faire des drivers dri histoire que tous les 'concurents' d'xfree puissent avoir leur accélératino graphique?
Les devel du kernel mettent en place ca et ben personne ne l'utilises
a par ceux qui font les drivers open source (et encore)
moi j'aimerais bien avoir la 3D accéléré sur ma carte nvidia quoi
ca serait sympa
(pas que la 3D accéléré si possible aussi la 2d genre la transparence;)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/11274.html">
Forum Francais pour fluxbox</a>

	<br>

<a href="http://linuxfr.org/~phhusson/11274.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Je vous anonce la création d'un des premiers (le premier?) forum consacre a fluxbox:
http://sidenux.ath.cx/phpBB2/viewforum.php?f=9(...)
voila c'est aussi court que ca mon journal
pour une fois ;)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/11030.html">
La LEN, lettre au président</a>

	<br>

<a href="http://linuxfr.org/~phhusson/11030.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

voila voila,
c'est pas de moi mais bon:
http://robertlan.eu.org/hebergeur.txt(...)
il parait que c'est un poeme
ceux qu'ils le voient me le disent ;)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/10993.html">
Frame buffer et vidéo intégré nforce2: vitcoire!!!!!!!!!!</a>

	<br>

<a href="http://linuxfr.org/~phhusson/10993.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Voila j'ai fait un tout petit patch pour noyau 2.6.4 pour ajouter le support de ma carte nforce2 accessible ici:http://sidenux.ath.cx/patch-nforce(...)
par contre il a pas l'air d'aimer fbcon.......
donc je vais l'utiliser que pour XDirectFB
aller ++ moi vais manger
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/10876.html">
Xdirectfb powaaaaa enfin........ quand le frame buffer pour ma nfrorce2 marrchera</a>

	<br>

<a href="http://linuxfr.org/~phhusson/10876.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Si vous lisez l'historique de mes commentaires vous verez que je dis regulierement (x)directfb powaa
seulement voila
je suis limite (a cause de cette *bip* de carte graphique intégrée a ma nforce 2) a etre en 800x600 à 60hz et avec le driver vesa
donc en gros la grosse bouse(enfin bon il arrive a faire tourner bankiz, une video et un xterm tout ca en transparence sans ramer http://sidenux.ath.cx/bankiz-matrix-irssi.png(...) ou encore http://sidenux.ath.cx/directfb-trans.png(...))
donc bon que l'acceleration materielle soit supportée je m'en fou un peu(sauf la partie 3D ;))
par contre j'aimerais bien pouvoir faire du 1600x1200 75hz ou du 1280x1024 a 85hz
parce que c'est quand meme frustrant d'etre en 800x600 a 60hz sur un ecran 19"
donc voila j'en appelle a vos connaissances pour faire marcher ce frame buffer (et pis avoir le bootsplash ca pourait etre zolil ;) (j'ai failli dire util)
donc c'est du NV18 si ca peut aider certain mais je doute
voila c'est tout ;)
	</div>



	<div class="text">

<a href="http://linuxfr.org/~phhusson/" title="Journal de Ph Husson">
phhusson</a>

:
<a href="http://linuxfr.org/~phhusson/10435.html">
Daemon bittorrent</a>

	<br>

<a href="http://linuxfr.org/~phhusson/10435.html">December 06, 2004 11:13 PM</a>


	</div>
	<div class="text">

Cher journal ,chères lectrices de journaux et chers lecteur de journaux,

Est-ce que l'un d'entre vous aurait la connaissance d'un daemon bittorrent qui gère la partie upload du client P2P
et coupler avec ceci, un client bittorrent pour telecharger des fichiers en ligne de commande comme un wget ou un curl

Seules petites contraintes:
le daemon le plus leger possible
et il doit aussi pouvoir gérer un très grand nombre de fichiers( au moins 100)
parce que j'ai vaguement essayer avec le client officiel
c'etait la cata :)

merci d'avance :)
	</div>

