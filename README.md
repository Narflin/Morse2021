# Convertisseur morse<->français
paul colombel

## Comment compiler
avoir `Audiofile.h` et `main.cpp` dans le même répertoire et compiler `main.cpp` sans options particulières.

## Comment l'utiliser
>### Du français vers le morse
>- Entrer  "`e`" comme encoder quand le programme le demande
>- Le programme demande un nom de fichier en .wav, il faut écrire un nom de fichier en .wav, il y a une vérification que le fichier n'existe pas déjà et que le nom entré est bien en .wav.
>- rentrer le texte à convertir quand demandé
>- le fichier .wav est généré dans le répertoire du programme

>### Du morse vers le français
>- Entrer "`d`" comme décoder quand le programme le demande
>- Entrer le chemin du fichier .wav
>- le programme peut renvoyer une erreur selon comment le fichier .wav est encodé, cela marche avec les fichiers créés par le programme et les quelques fichiers d'autres membres du groupe C++ que j'ai pu tester.
>- le programme s'ajuste automatiquement sur la fréquence du son et la vitesse d'émission du morse
>- Si le fichier est correct, le programme renvoie le texte converti (avec un message particulier si des charractères sont non reconnus)

## Charactères supportés
> Les lettres A-Z, les chiffres 0-9, et les ponctuations suivantes :
> 
>. , ? ' ! / ( ) & : ; = + - _



