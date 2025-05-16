🏥 The Forsaken Hospital - OOP Project 🩸
A horror escape room game built in C++
📖 Descriere
The Forsaken Hospital este un joc RPG de tip escape room horror, în care jucătorul este prins într-un spital abandonat. Caracterul se trezește blocat în acest spital abandonat, iar pentru a reuși să evadeze trebuie să treacă prin mai multe camere diferite:

Sală de operații
Partea de sanatoriu
Arhiva
Saloane
Camera de medicamente
Etc.

În fiecare cameră, caracterul poate găsi diferite iteme și puzzle-uri:

Itemele pot fi colectabile și pot ajuta caracterul în luptele cu inamicii, îi pot reda din viață și pot ajuta la deschiderea ușii principale pentru a evada
Puzzle-urile pot fi rezolvate de jucător pentru a primi ca recompensă noi iteme

Pe parcursul explorării camerelor, caracterul se poate întâlni cu diferite entități malefice, cu care trebuie să se lupte pentru a le distruge. După ce sunt traversate toate camerele, sunt distruși toți inamicii și sunt colectate toate itemele, caracterul poate evada din spitalul abandonat, câștigând astfel jocul.
Show Image
🎮 Caracteristici principale

🏚️ Explorare interactivă a unui spital înfricoșător
🧩 Puzzle-uri și obiecte ascunse
👻 Inamici periculoși care te urmăresc
⚔️ Sistem de luptă cu iteme și strategii
🔦 Inventar gestionabil cu obiecte utile

📋 Cerințele principale ale proiectului
✅ Partea 1

 Implementarea în C++
 Folosirea a minim 4 clase (Item, Entity, Character, Room, Game)
 Fiecare clasă are constructori de inițializare (cu și fără parametri), constructor de copiere, destructor, operator de atribuire, setteri și getteri pentru toate datele
 Supraîncărcarea operatorilor de citire și scriere
 Să existe metode publice prin care se realizează citirea și afișarea informațiilor complete a n obiecte, memorarea și afișarea acestora (Display, Read)
 Meniu interactiv
 Proiectul să ruleze la momentul prezentării

✅ Partea 2
Moșteniri

Multiple:

Riddle și HangMan sunt moștenite din clasa abstractă Puzzle
AttackItem, HealItem și CollectibleItem sunt moștenite din clasa abstractă Item
Enemy este moștenită din clasa abstractă Entity


În lanț:

SpecialEnemy moștenește clasele Enemy și AttackItem



Alte cerințe

 Clase abstracte (Item, Entity, Puzzle)
 Funcții virtuale apelate prin pointeri la clasa de bază
 Static și const
 UPCASTING și DOWNCASTING
 Tratare excepții
 STL
 Implementarea a cel puțin două funcționalități noi specifice temei
 Fără variabile globale sau funcții independente
 Clean code

✅ Partea 3
Template-uri

Funcție generică: pentru a adăuga un obiect într-o colecție de obiecte
Clasă Container: pentru a gestiona colecțiile de elemente
Clasă ReadValidatedInput: folosită pentru a verifica dacă input-ul este corect

Design Pattern-uri

BEHAVIORAL → Observer Pattern
Observer, Subject, ScoreManager
Cuplarea dintre subiect și observator, folosită la jocurile de Riddle și HangMan (notifică player-ul, gestionează scorul și activitatea jocurilor)
STRUCTURAL → Decorator Pattern
GameItem, ItemDecor, ExperienceBoostDecorator, 
DurationEffectDecorator, ItemAdapter
Oferă o decorare specială a itemelor (efecte speciale)
CREATIONAL → Factory Pattern
PuzzleFactory, RiddleFactory, HangManFactory
Oferă o interfață pentru crearea obiectelor de tip Puzzle
SINGLETON → Singleton Pattern
Utilities
Pentru înglobarea funcțiilor din cod independente

🚀 Cum să rulezi proiectul
bash# Clonează repository-ul
git clone [URL-repo]

# Navighează în directorul proiectului
cd the-forsaken-hospital

# Compilează proiectul
g++ -std=c++17 -o game main.cpp [alte-fisiere.cpp]

# Rulează jocul
./game
🛠️ Tehnologii folosite

Limbaj: C++17
Paradigme: Programare Orientată pe Obiecte
Design Patterns: Observer, Decorator, Factory, Singleton
STL: Colecții și algoritmi standard

📝 Structura proiectului
The-Forsaken-Hospital/
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   ├── entities/
│   │   ├── Character.cpp
│   │   ├── Enemy.cpp
│   │   └── ...
│   ├── items/
│   │   ├── Item.cpp
│   │   ├── AttackItem.cpp
│   │   └── ...
│   ├── rooms/
│   │   ├── Room.cpp
│   │   └── ...
│   └── puzzles/
│       ├── Puzzle.cpp
│       ├── Riddle.cpp
│       └── ...
├── headers/
│   └── ...
├── assets/
│   └── harta_spital.png
└── README.md
👥 Echipa

[Numele tău]

📄 Licență
Acest proiect este dezvoltat în scop educațional pentru cursul de Programare Orientată pe Obiecte.

⭐ Dacă ți-a plăcut proiectul, lasă un star!
