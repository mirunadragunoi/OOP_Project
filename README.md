# 🏥 The Forsaken Hospital - OOP_Project 🩸  
_A horror escape room game built in C++._

---

## 📖 Descriere
**The Forsaken Hospital** este un joc RPG de tip escape room horror, în care jucătorul este prins într-un spital abandonat. Caracterul se trezește blocat în acest spital abandonat iar pentru a reuși să evadeze trebuie să treacă prin mai multe camere diferite (sală de operații, partea de sanatoriu, arhiva, saloane, camera de medicamente, etc). În fiecare cameră caracterul poate găsi diferite iteme și puzzle-uri. Itemele pot fi colectabile și pot ajuta caracterul în luptele cu inamicii, îi pot reda din viață și pot ajuta la deschiderea ușii principale pentru a evada. Puzzle-urile pot fi rezolvate de jucător și pot primi ca recompensă un nou item după placul lui. Pe parcursul explorării camerelor, caracterul se poate întâlni cu diferite entități malefice, cu care trebuie să se lupte pentru a le distruge. După ce sunt traversate toate camerele, sunt distruși toți inamicii și sunt colectate toate itemele, caracterul poate evada din spitalul abandonat, câștigând astfel jocul.  

---

## 🎮 Caracteristici principale  
- 🏚️ Explorare interactivă a unui spital înfricoșător  
- 🧩 Puzzle-uri și obiecte ascunse
- 👻 Inamici periculoși care te urmăresc  
- ⚔️ Sistem de luptă cu iteme și strategii  
- 🔦 Inventar gestionabil cu obiecte utile

---

## ✅ Cerințele principale ale proiectului:
### 📌 _Partea 1_
  - [x] Implementarea în C++
  - [x] Folosirea a minim 4 clase (_Item_, _Entity_, _Character_, _Room_, _Game_)
  - [x] Fiecare clasă are constructori de inițializare (cu și fără parametri), constructor de copiere, destructor, operator de atribuire, setteri și getteri pentru toate datele
  - [x] Supraîncărcarea operatorilor de citire și scriere
  - [x] Să existe metode publice prin care se realizează citirea și afișarea informațiilor complete a n obiecte, memorarea și afișarea acestora (_Display_, _Read_)
  - [x] Meniu interactiv
  - [x] Proiectul să ruleze la momentul prezentării
       
### 📌 _Partea 2_
  - [x] Moșteniri
     - **multiple**:
           - _Riddle_ și _HangMan_ sunt moștenite din clasa abstractă _Puzzle_
           - _AttackItem_, _HealIteam_ și _CollectibleItem_ sunt moștenite din clasa abstractă _Item_
           -  _Enemy_ este moștenită din clasa abstractă _Entity_
      - **în lanț:** - _SpecialEnemy_ moștenește clasele _Enemy_ și _AttackItem_
  - [x] Clase abstracte (_Item_, _Entity_, _Puzzle_)
  - [x] Funcții virtuale apelate prin pointeri la clasa de bază
  - [x] Static și const
  - [x] UPCASTING și DOWNCASTING
  - [x] Tratare excepții
  - [x] STL
  - [x] Implementarea a cel puțin două funcționalități noi specifice temei
  - [x] Fără variabile globale sau funcții independente
  - [x] Clean code
        
### 📌 _Partea 3_
  - [x] Implementarea a 2 templete-uri
    - funcție generică de a adăuga un obiect într-o colecție de obiecte
    - clasă generică Container pentru a gestiona colecțiile de elemente
    - clasă generică ReadValidatedInput folosită pentru a verifica dacă input-ul este corect
  - [x] Implementarea a 3 design pattern-uri
    - 🧠 **BEHAVIORAL** -> _Observer_, _Subject_ și _ScoreManager_: cuplarea dintre subiect și observator, folosită la jocurile de _Riddle_ și _HangMan_(notifică player-ul, gestionează scorul și activitatea jocurilor).
    - 🧱 **STRUCTURAL** -> _GameItem_, _ItemDecor_, _ExperienceBoostDecorator_, _DurationEffectDecorator_, _ItemAdapter_: oferă o decorare specială a itemelor (efecte speciale).
    - 🏭 **CREAȚIONAL** -> _PuzzleFactory_, _RiddleFactory_, _HangManFactory_: oferă o interfață pentru crearea obiectelor de tip _Puzzle_.
    - 🧍 **SINGLETON** -> _Utilities_: pentru înglobarea funcțiilor din cod independente.

---

## 🛠️ Tehnologii folosite
- **Limbaj**: C++20  
- **Paradigme**: Programare Orientată pe Obiecte  
- **Design Patterns**: Observer, Decorator, Factory, Singleton  
- **STL**: Colecții și algoritmi standard  

--- 

## 🚀 Cum să rulezi proiectul

```bash
# Clonează repository-ul
git clone [URL-repo]

# Navighează în directorul proiectului
cd the-forsaken-hospital

# Compilează proiectul
g++ -std=c++17 -o game main.cpp [alte-fisiere.cpp]

# Rulează jocul
./game
```

## 📄 Licență

Acest proiect este dezvoltat în scop educațional pentru cursul de **Programare Orientată pe Obiecte**.
