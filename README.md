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
>- le programme utilise une librairie pour la lecture du fichier, elle vérifie que le Header est bien conforme au .wav, et que les donnés du header soient cohérentes avec les donnés du fichier, si ce n'est pas le cas une erreur est renvoyée (assez locace sur le type de problème du Header).
>- Si le fichier est correct, le programme renvoie le texte converti (avec un message particulier si des charractères sont non reconnus)

## Charactères supportés
> Les lettres A-Z, les chiffres 0-9, et les ponctuations suivantes :
> 
>. , ? ' ! / ( ) & : ; = + - _



