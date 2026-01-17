# SYST_B
Projet__Syst_Electronique

Start : mode veille :
si (détection < 15cm) -> mode prepa gen

Prepa gen : 
si bp arm -> mode prepa act
si (t>40s) -> retour mode veille

Prepa act :
si bp arm -> mode prepa gen
si bp auto -> si bpconf -> mode arm auto
si bp retard -> mode prepa retard code

Prepa retard code :
si bp arm -> mode prepa gen
    "#" confirmer saisie
    "*" effacer dernier caractère
    (autre) ajouté à combinaison
choix code -> confirmation code -> mode prepa retard

Prepa retard :
si bp arm -> mode prepa gen
    choix jour :
    (num) ajout num (ex : j + (num)) max j + 9
    "*" effacer dernier carctère
    "#" confirme date
    
    choix heure :
    format hh:mm
    même fonctionnement
    
    confirm :
    si bp conf -> arm retard

Arm auto :
si detection -> 5s -> mode boom

Arm retard:
si detection -> ouverture, code désamorçage
    
