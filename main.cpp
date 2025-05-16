#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <map>

class Item;
class Character;
class Puzzle;
class Room;
class Game;

/*
 *    Short Description:
 *         Am implementat un joc RPG horror in care personajul trebuie sa evadeze dintr-un spital abandonat. Caracterul
 *    meu se trezeste blocat in acest spital iar pentru a reusi sa evadeze trebuie sa treaca prin mai multe camere
 *    diferite (sala de operatii, partea de sanatoriu, arhiva, saloane, camera de medicamente). In fiecare camera
 *    caracterul poate gasi diferite iteme, puzzle-uri si medicamente. Itemele pot fi colectabile si pot ajuta
 *    personajul sa evadeze. Puzzle-urile pot fi rezolvate de jucator iar ca recompensa pot primi un item nou. In
 *    acest timp, caracterul meu are si un nivel de viata (sanatate), care poate scadea pe parcursul explorarii
 *    camerelor, de aceea medicamentele ii pot reda din sanatate. Deoarece este un spital abandonat iar jocul este unul
 *    de tip horror, in camere se pot intalni diferite entitati, ele fiind periculoase, agresive sau pasive (cum ar fi
 *    fantome, monstrii, fantomele criminalilor in serie, etc). Acestea pot ataca personajul meu, scazandu-i din viata,
 *    iar caracterul de poate lupta cu ele pentru a le distruge. Dupa ce jucatorul trece prin toate camerele, rezolva
 *    puzzle-urile si colecteaza itemele necesare, acesta poate deschide usa principala si poate evada din spiral,
 *    castingand astfel jocul. In caz contrar, caracterul o sa-si piarda toata viata, va muri si va pierde jocul.
 */


//      Definesc clasa abstracta Item. Acestea pot fi:
//          1. Item colectabil - poate fi colectat de jucator si il va ajuta la final sa deschida usa pentru a evada
//          2. Item de aparare - este colectat de jucator si il va ajuta sa invinga o creatura malefica
//          3. Item de viata - medicamente, ii pot reda din viata jucatorului
//          Toate acestea 3 vor deveni clase derivate.
//
//      !!! MOSTENIRE IN LANT
//
//      BONUS:
//              Prin operatia de add a doua obiecte de acelasi tip (atac sau heal), pot crea un nou obiect ce genereaza
//           mai mult damage sau da un heal mai mare jucatorului.


// functie de tip template pentru adaugarea generica a unui obiect intr un vector (orice obiect, indiferent de tip)

template<typename T>
void AddToVector(std::vector<std::shared_ptr<T>>& vector, const std::shared_ptr<T>& element) {
    vector.push_back(element);
}

// clasa de tip template numita Container pentru a gestiona colectiile de obiecte (iteme, entitati, camere, etc)

template<typename T>
class Container {
private:
    std::vector<std::shared_ptr<T>> elements;
public:
    void AddElement(const std::shared_ptr<T>& element) {
        elements.push_back(element);
    }

    void RemoveElement(int index) {
        if (index >= 0 && index < elements.size())
            elements.erase(elements.begin() + index);
    }

    std::shared_ptr<T> GetElement(int index) const {
        if (index >= 0 && index < elements.size()) {
            return elements[index];
        }
        else {
            std::cout << "      Invalid index!" << std::endl;
        }
        return nullptr;
    }

    size_t GetSize() const {
        return elements.size();
    }

    bool ContainerIsEmpty() const {
        return elements.empty();
    }

    void DisplayAllElements() const {
        if (elements.empty()) {
            std::cout << "      Inventory is empty." << std::endl;
            return;
        }

        std::cout << "      Inventory contains:" << std::endl;
        for (size_t i = 0; i < elements.size(); i++) {
            std::cout << "      " << i + 1 << ". " << *elements[i] << std::endl;
        }
    }

    void ClearContainer() {
        elements.clear();
    }
};

// definesc o clasa template ce va fi utilizata in meniu pentru input urile valide
template <typename T>
T ReadValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) break;
        else {
            std::cin.clear();
            std::cin.ignore(100, '\n');
            std::cout << "      Invalid input. Try again!" << std::endl;
        }
    }
    std::cin.ignore();
    return value;
}

class Item {
protected:
    std::string item_name;

public:
    // constructorul fara parametrii
    Item() {
        this->item_name = "";
    }

    // constructorul parametrizat
    Item(const std::string& name) {
        this->item_name = name;
    }

    // constructorul de copiere
    Item(const Item& item) {
        this->item_name = item.item_name;
    }

    // constructorul de atribuire
    Item& operator=(const Item& item) {
        if (this != &item) {
            this->item_name = item.item_name;
        }
        return *this;
    }

    // supraincarcarea operatorului de add + => functie pur virtuala
    virtual Item* operator+(const Item& newitem) const = 0;


    // metoda publica de citire a datelor => functie pur virtuala
    virtual void ReadItemInfo(std::istream& in) = 0;

    // metoda publica de afisare a datelor => functie pur virtuala
    virtual void DisplayItemInfo(std::ostream& out) const = 0;

    // metoda de copiere a unui item => functie pur virtuala
    virtual Item* Clone() const = 0;

    // supraincarcarea operatorului de scriere
    friend std::ostream& operator<<(std::ostream& out, const Item& item) {
        item.DisplayItemInfo(out);
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, Item& item) {
        item.ReadItemInfo(in);
        return in;
    }

    // getteri
    const std::string& GetItemName() {
        return this->item_name;
    }

    // setteri
    void SetItemName(const std::string& name) {
        this->item_name = name;
    }

    // destructor
    virtual ~Item() = default;
};

// definesc clasa derivata AttackItem (derivata din clasa abstracta Item)
// este responsabila de itemele cu care caracterul poate ataca entitarile malefice

class AttackItem : virtual public Item {
protected:
    std::string attack_item_type;
    int attack_value;

public:
    //constructorul
    explicit AttackItem(const std::string& name = "", const std::string& attack_item_type= "", int attack_value = 0)
        : Item(name), attack_item_type(attack_item_type), attack_value(attack_value) {}

    // constructorul de copiere
    AttackItem(const AttackItem& attack_item)  : Item(attack_item) {
        this->item_name = attack_item.item_name;
        this->attack_item_type = attack_item.attack_item_type;
        this->attack_value = attack_item.attack_value;
    }

    // operatorul de atribuire
    AttackItem& operator=(const AttackItem& attack_item) {
        if (this != &attack_item) {
            this->item_name = attack_item.item_name;
            this->attack_item_type = attack_item.attack_item_type;
            this->attack_value = attack_item.attack_value;
        }
        return *this;
    }

    // supraincarcarea operatorului de add +
    Item* operator+(const Item& newItem) const override {
        // DOWNCASTING - verific daca itemul poate fi "AttackItem"
        const AttackItem* attackPtr = dynamic_cast<const AttackItem*>(&newItem);

        if (attackPtr) {
            // concatenez numele
            std::string newName = this->item_name + " & " + attackPtr->item_name;

            // concatenez tipurile
            std::string newType = this->attack_item_type + " & " + attackPtr->attack_item_type;

            // adun valorile
            int newValue = this->attack_value + attackPtr->attack_value +
                0.25 * (this->attack_value + attackPtr->attack_value);

            // returnez noul obiect creat prin adaugare
            return new AttackItem(newName, newType, newValue);
        }

        // daca nu e AttackItem, arunc o exceptie
        throw std::invalid_argument("   You cannot stack an attack item with an item of another type!");
    }

    // definesc metoda de copiere a unui obiect
    Item* Clone() const override {
        return new AttackItem(*this);
    }

    // supraincarcarea operatorului de citire
    void ReadItemInfo(std::istream& in) override {
        std::cout << "  Enter the attack item name: ";
        in >> this->item_name;
        std::cout << "  Enter the attack item type: ";
        in >> this->attack_item_type;
        std::cout << "  Enter the damage (value) of attack item: ";
        in >> this->attack_value;
    }

    // supraincarcarea operatorului de scriere
    void DisplayItemInfo(std::ostream& out) const override {
        out << "    Attack Item: (name: " << this->item_name << ")" << " & (type: " << this->attack_item_type <<
            ")" << " & (damage: " << this->attack_value << ")" << std::endl;
    }

    // getteri
    const std::string GetAttackItemName() {
        return this->item_name;
    }
    const std::string GetAttackItemType() {
        return this->attack_item_type;
    }
    const int GetAttackItemValue() {
        return this->attack_value;
    }

    // setteri
    void SetAttackItemName(const std::string& name) {
        this->item_name = name;
    }
    void SetAttackItemType(const std::string& type) {
        this->attack_item_type = type;
    }
    void SetAttackItemValue(int value) {
        this->attack_value = value;
    }

    ~AttackItem() override = default;
};

//      Definesc clasa derivata HealIteam (derivata din clasa de baza/abstracta Item)
//    => este responsabila de itemele ce ii vor reda din viata caracterului

class HealItem : virtual public Item {
protected:
    std::string type;
    int heal_value;

public:
    // constructorul
    HealItem(std::string name = "", const std::string& type = "", int heal_value = 0)
        : Item(name), type(type), heal_value(heal_value) {}

    // constroctuorul de copiere
    HealItem(const HealItem& heal_item)  : Item(heal_item) {
        this->item_name = heal_item.item_name;
        this->type = heal_item.type;
        this->heal_value = heal_item.heal_value;
    }

    // operatorul de atribuire
    HealItem& operator=(const HealItem& heal_item) {
        if (this != &heal_item) {
            this->item_name = heal_item.item_name;
            this->type = heal_item.type;
            this->heal_value = heal_item.heal_value;
        }
        return *this;
    }

    // supraincarcarea operatorului de add +
    Item* operator+(const Item& newItem) const override {
        // DOWNCASTING - verific daca itemul poate fi "HealItem"
        const HealItem* healPtr = dynamic_cast<const HealItem*>(&newItem);

        if (healPtr) {
            // concatenez numele
            std::string newName = this->item_name + " & " + healPtr->item_name;

            // concatenez tipurile
            std::string newType = this->type + " & " + healPtr->type;

            // adun valorile
            int newValue = this->heal_value + healPtr->heal_value +
                0.25 * (this->heal_value + healPtr->heal_value);

            // returnez noul obiect creat prin adaugare
            return new HealItem(newName, newType, newValue);
        }

        // daca nu e HealItem, arunc o exceptie
        throw std::invalid_argument("   You cannot stack an attack item with an item of another type!");
    }

    // definesc metoda de copiere a unui obiect
    Item* Clone() const override {
        return new HealItem(*this);
    }

    // supraincarcarea operatorului de citire
    void ReadItemInfo(std::istream& in) override {
        std::cout << "  Enter the heal item name: ";
        in >> this->item_name;
        std::cout << "  Enter the heal item type: ";
        in >> this->type;
        std::cout << "  Enter the value of heal item: ";
        in >> this->heal_value;
    }

    // supraincarcarea operatorului de scriere
    void DisplayItemInfo(std::ostream& out) const override {
        out << "    Heal item: (name: " << this->item_name << ")" << " & (type: " << this->type <<
            ") & (heal_value: " << this->heal_value << ")" << std::endl;
    }

    // getteri
    const std::string GetHealItemName() {
        return this->item_name;
    }
    const std::string GetHealItemType() {
        return this->type;
    }
    const int GetHealItemValue() {
        return this->heal_value;
    }

    // setteri
    void SetHealItemName(const std::string& name) {
        this->item_name = name;
    }
    void SetHealItemType(const std::string& type) {
        this->type = type;
    }
    void SetHealItemValue(int value) {
        this->heal_value = value;
    }

    virtual ~HealItem() = default;
};

//      Definesc clasa derivata CollectibleIteam (derivata din clasa de baza/abstracta Item)
//    => este responsabila de itemele colectabile ce il vor ajuta pe caracter sa treaca dintr o camera in alta si
//    sa deschida usa principala pentru a evada

class CollectibleItem : virtual public Item {
protected:
    std::string type;
    std::string rarity;

public:
    // constructorul
    explicit CollectibleItem(std::string name = "", const std::string& type = "", const std::string& rarity = "")
        : Item(name), type(type), rarity(rarity) {}

    // constroctuorul de copiere
    CollectibleItem(const CollectibleItem& collectible_item)  : Item(collectible_item) {
        this->item_name = collectible_item.item_name;
        this->type = collectible_item.type;
        this->rarity = collectible_item.rarity;
    }

    // operatorul de atribuire
    CollectibleItem& operator=(const CollectibleItem& collectible_item) {
        if (this != &collectible_item) {
            this->item_name = collectible_item.item_name;
            this->type = collectible_item.type;
            this->rarity = collectible_item.rarity;
        }
        return *this;
    }

    // supraincarcarea operatorului de add +
    Item* operator+(const Item& newItem) const override {
        // DOWNCASTING - verific daca itemul poate fi "CollectibleItem"
        const CollectibleItem* collectiblePtr = dynamic_cast<const CollectibleItem*>(&newItem);

        if (collectiblePtr) {
            // concatenez numele
            std::string newName = this->item_name + " & " + collectiblePtr->item_name;

            // concatenez tipurile
            std::string newType = this->type + " & " + collectiblePtr->type;

            // adun valorile
            std::string newValue = this->rarity + collectiblePtr->rarity;

            // returnez noul obiect creat prin adaugare
            return new CollectibleItem(newName, newType, newValue);
        }

        // daca nu e CollectibleItem, arunc o exceptie
        throw std::invalid_argument("   You cannot stack an attack item with an item of another type!");
    }

    // definesc metoda de copiere a unui obiect de tip Collectible Item
    Item* Clone() const override {
        return new CollectibleItem(*this);
    }

    // supraincarcarea operatorului de citire
    void ReadItemInfo(std::istream& in) override {
        std::cout << "  Enter the collectible item name: ";
        in >> this->item_name;
        std::cout << "  Enter the collectible item type: ";
        in >> this->type;
        std::cout << "  Enter the collectible item rarity: ";
        in >> this->rarity;
    }

    // supraincarcarea operatorului de scriere
    void DisplayItemInfo(std::ostream& out) const override {
        out << "    Collectible item: (name: " << this->item_name << ") &" << " (type: " << this->type <<
            ") & (rarity: " << this->rarity << ")" << std::endl;
    }

    // getteri
    const std::string GetCollectibleItemName() {
        return this->item_name;
    }
    const std::string GetCollectibleItemType() {
        return this->type;
    }
    const std::string GetCollectibleItemRarity() {
        return this->rarity;
    }

    // setteri
    void SetCollectibleItemName(const std::string& name) {
        this->item_name = name;
    }
    void SetCollectibleItemType(const std::string& type) {
        this->type = type;
    }
    void SetCollectibleItemRarity(const std::string& rarity) {
        this->rarity = rarity;
    }

    ~CollectibleItem() override = default;
};

//      Definesc clasa abstracta Entity, din aceasta vor fi derivate alte doua clase:
//          1. Enemy -> entitatile malefice ce vor ataca playerul pe parcursul jocului (caracterul se pot lupta cu ele)
//          2. NPC -> entitatile inofensive ce vor fi prezente in camere, dar nu fac niciun rau playerului
//                 -> acestea pot oferi anumite sfaturi playerului despre joc -> unde se afla anumite iteme
//
//      !!! MOSTENIRE IN LANT

// definesc clasa abstracta Entity

class Entity {
protected:
    std::string entity_name;
    int health;

public:
    // constructorul fara parametrii
    Entity() {
        this->entity_name = "Empty";
        this->health = 0;
    }

    // constructorul cu parametrii
    Entity(const std::string& name, int health) {
        this->entity_name = name;
        this->health = health;
    }

    // constructorul de copiere
    Entity(const Entity& entity) {
        this->entity_name = entity.entity_name;
        this->health = entity.health;
    }

    // operatorul de atribuire
    Entity& operator=(const Entity& entity) {
        if (this != &entity) {
            this->entity_name = entity.entity_name;
            this->health = entity.health;
        }
        return *this;
    }

    // metoda publica de citire a unui obiect -> functie pur virtuala
    virtual void ReadEntityInfo(std::istream& in) = 0;

    // metoda publica de afisare a datelor despre un obiect -> functie pur virtuala
    virtual void DisplayEntityInfo(std::ostream& out) const = 0;

    // supraincarcarea operatorului de afisare (scriere)
    friend std::ostream& operator<<(std::ostream& out, const Entity& entity) {
        entity.DisplayEntityInfo(out);
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, Entity& entity) {
        entity.ReadEntityInfo(in);
        return in;
    }

    // getteri
    const std::string GetEntityName() {
        return this->entity_name;
    }
    int GetEntityHealth() {
        return this->health;
    }

    // setteri
    void SetEntityName(const std::string& name) {
        this->entity_name = name;
    }
    void SetEntityHealth(int health) {
        this->health = health;
    }

    // destructorul -> specific virtual
    virtual ~Entity() = default;
};

//      Definesc clasa derivata Enemy -> derivata din clasa de baza / abstracta Entity, responsabila de entitatile
//    malefice ce vor ataca playerul pe parcursul jocului, in timp ce acesta exploreaza camerele.

class Enemy : virtual public Entity {
protected:
    int attack_damage;
    std::string enemy_type;
    int level;

public:
    // constructorul
    explicit Enemy(const std::string& name = "", int health = 0, int attack_damage = 0,
        const std::string& type = "", int level = 0)
            : Entity(name, health), attack_damage(attack_damage), enemy_type(type), level(level) {}

    // constructorul de copiere
    Enemy(const Enemy& enemy)  : Entity(enemy) {
        this->entity_name = enemy.entity_name;
        this->health = enemy.health;
        this->attack_damage = enemy.attack_damage;
        this->enemy_type = enemy.enemy_type;
        this->level = enemy.level;
    }

    // operatorul de atribuire
    Enemy& operator=(const Enemy& enemy) {
        if (this != &enemy) {
            this->entity_name = enemy.entity_name;
            this->health = enemy.health;
            this->attack_damage = enemy.attack_damage;
            this->enemy_type = enemy.enemy_type;
            this->level = enemy.level;
        }
        return *this;
    }

    // supraincarcarea operatorului de citire
    void ReadEntityInfo(std::istream& in) override {
        std::cout << "  Enter the enemy name: ";
        in >> this->entity_name;
        std::cout << "  Enter the enemy health: ";
        in >> this->health;
        std::cout << "  Enter the enemy attack damage: ";
        in >> this->attack_damage;
        std::cout << "  Enter the enemy type: ";
        in >> this->enemy_type;
        std::cout << "  Enter the enemy level: ";
        in >> this->level;
    }

    // supraincarcarea operatorului de afisare / scriere
    void DisplayEntityInfo(std::ostream& out) const override {
        out << "    Enemy: (name: " << this->entity_name << ") & (health: " << this->health <<
            ") & (attack damage: " << this->attack_damage << ") & (type: " << this-> enemy_type
            << ") & (level: " << this->level << ")" << std::endl;
    }

    // getteri
    const std::string& GetEnemyName() {
        return this->entity_name;
    }
    int GetEnemyHealth() {
        return this->health;
    }
    int GetEnemyAttackDamage() const {
        return this->attack_damage;
    }
    const std::string& GetEnemyType() {
        return this->enemy_type;
    }
    int GetEnemyLevel() const {
        return this->level;
    }

    // setteri
    void SetEnemyName(const std::string& name) {
        this->entity_name = name;
    }
    void SetEnemyHealth(int health) {
        this->health = health;
    }
    void SetEnemyAttackDamage(int attack_damage) {
        this->attack_damage = attack_damage;
    }
    void SetEnemyType(std::string type) {
        this->enemy_type = type;
    }
    void SetEnemyLevel(int level) {
        this->level = level;
    }

    // functie pentru a verifica daca enemy ul a fost invins
    bool EnemyIsDefeated() const {
        if (this->health <= 0) {
            return true;
        }
        return false;
    }

    ~Enemy() override = default;
};

//        Definesc o clasa speciala, numita SpecialEnemy, care mosteneste clasa derivata Enemy (derivata din clasa
//    de baza/abstracta Entity) si clasa derivata AttackItem (derivata din clasa de baza/abstracta Item)
//    (MOSTENIRE MULTIPLA). Acest adversar va avea in componenta sa si un item ce ii va creste puterea
//    de damage intr o lupta cu player-ul.

class SpecialEnemy: public Enemy, public AttackItem {
public:
    // constructor
    explicit SpecialEnemy(const std::string& enemy_name = "", int health = 0, int attack_damage = 0,
    const std::string& enemy_type = "", int level = 0, const std::string& item_name = "",
    const std::string& attack_item_type = "", int attack_value = 0)
    : Entity(enemy_name, health), Enemy(enemy_name, health, attack_damage, enemy_type, level), // apelez destructorul clasei Entity
      Item(item_name), AttackItem(item_name, attack_item_type, attack_value) {}     // apelez desturctorul clasei AttackItem

    // constructorul de copiere
    SpecialEnemy(const SpecialEnemy& special_enemy)  : Entity(special_enemy), Item(special_enemy) {
        this->entity_name = special_enemy.entity_name;
        this->health = special_enemy.health;
        this->attack_damage = special_enemy.attack_damage;
        this->enemy_type = special_enemy.enemy_type;
        this->level = special_enemy.level;
        this->item_name = special_enemy.item_name;
        this->attack_item_type = special_enemy.attack_item_type;
        this->attack_value = special_enemy.attack_value;
    }

    // supraincarcarea operatorului de atribuire
    SpecialEnemy& operator=(const SpecialEnemy& special_enemy) {
        if (this != &special_enemy) {
            // apelez operatorii de atribuire ai bazelor
            Enemy::operator=(special_enemy);
            AttackItem::operator=(special_enemy);
        }
        return *this;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, SpecialEnemy& special_enemy) {
        special_enemy.ReadEntityInfo(in);   // citirea de la clasa Enemy
        special_enemy.ReadItemInfo(in);         // citirea de la clasa AttackItem
        return in;
    }

    // supraincarcarea operatorului de scriere
    friend std::ostream& operator<<(std::ostream& out, const SpecialEnemy& special_enemy) {
        out << "       --------------------------" << std::endl;
        out << "        Special Enemy: " << special_enemy.GetSpecialEnemyName()<< std::endl;
        special_enemy.DisplayEntityInfo(out);    // afisarea de la clasa Enemy
        special_enemy.DisplayItemInfo(out);      // afisarea de la clasa AttackItem
        out << "       --------------------------" << std::endl;
        return out;
    }

    // definesc metoda de citire a datelor
    void ReadSpecialEnemyInfo() {
        std::cin >> *this;
    }

    // definesc metoda de afisare a datelor
    void DisplaySpecialEnemyInfo() const {
        std::cout << *this;
    }

    // getteri
    std::string GetSpecialEnemyName() const {
        return this->entity_name;
    }
    int GetSpecialEnemyHealth() const {
        return this->health;
    }
    int GetSpecialEnemyAttackDamage() const {
        return this->attack_damage;
    }
    std::string GetSpecialEnemyType() const {
        return this->enemy_type;
    }
    int GetSpecialEnemyLevel() const {
        return this->level;
    }
    std::string GetSpecialEnemyItemName() const {
        return this->item_name;
    }
    std::string GetSpecialEnemyItemType() const {
        return this->attack_item_type;
    }
    int GetSpecialEnemyAttackItemValue() const {
        return this->attack_value;
    }

    // setteri
    void SetSpecialEnemyName(const std::string& name) {
        this->entity_name = name;
    }
    void SetSpecialEnemyHealth(int health) {
        this->health = health;
    }
    void SetSpecialEnemyAttackDamage(int attack_damage) {
        this->attack_damage = attack_damage;
    }
    void SetSpecialEnemyType(const std::string type) {
        this->enemy_type = type;
    }
    void SetSpecialEnemyLevel(int level) {
        this->level = level;
    }
    void SetSpecialEnemyItemName(const std::string& name) {
        this->item_name = name;
    }
    void SetSpecialEnemyItemType(const std::string type) {
        this->attack_item_type = type;
    }
    void SetSpecialEnemyAttackItemValue(int value) {
        this->attack_value = value;
    }

    // destructor
    ~SpecialEnemy() override = default;
};

//      Definesc clasa Character -> responsabila de player ul jocului.

class Character {
private:
    std::string name;  // numele jucatorului
    int health;
    int score;
    Container<Item> inventory;  // inventarul jucatorului
public:
    // constructorul fara parametrii
    Character(){
        this->name = "Player";
        this->health = 100;
        this->score = 0;
    };

    // definesc constructorul parametrizat
    Character(const std::string& name, int health, int score) {
        this->name = name;
        this->health = health;
        this->score = score;
    }

    // constructorul de copiere
    Character(const Character& character) {
        this->name = character.name;
        this->health = character.health;
        this->score = character.score;

        // copiez vectorul de iteme
        this->inventory = character.inventory;
    }

    // operatorul de atribuire
    Character& operator=(const Character& character) {
        if (this != &character) {
            this->name = character.name;
            this->health = character.health;
            this->score = character.score;
            this->inventory = character.inventory;
        }
        return *this;
    }

    // supraincarcarea operatorului de scriere
    friend std::ostream& operator<<(std::ostream& out, const Character& character) {
        out << "    Your character's name: " << character.name << std::endl;
        out << "    Your character's health: " << character.health << std::endl;
        out << "    Your character's score: " << character.score << std::endl;
        out << "    Your character's inventory size: " << character.inventory.GetSize() << std::endl;
        if (!character.inventory.ContainerIsEmpty()) {
            out << "    Your character's inventory items: " << std::endl;
            for (int i = 0 ; i <= character.inventory.GetSize() ; i++) {
                out << *character.inventory.GetElement(i) << std::endl;
            }
            out << std::endl;
        }
        else {
            out << "    Inventory items is empty." << std::endl;
        }
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, Character& character) {
        std::cout << "Enter your character's name: ";
        in >> character.name;
        std::cout << "Enter your character's health: ";
        in >> character.health;
        std::cout << "Enter your character's score: ";
        in >> character.score;
        return in;
    }

    // definesc metoda de citire a datelor
    void ReadCharacter() {
        std::cin >> *this;
    }

    // definesc metoda de scriere a datelor
    void DisplayInfoCharacter() const {
        std::cout << *this;
    }

    // getterii
    const std::string& GetCharacterName() const {
        return this->name;
    }

    int GetCharacterHealth() const {
        return this->health;
    }

    int GetCharacterScore() const {
        return this->score;
    }

    int GetInventorySize() const {
        return static_cast<int>(inventory.GetSize());
    }

    std::shared_ptr<Item> GetCharacterInventoryItem(int index) const {
        return inventory.GetElement(index);
    }

    // setterii
    void SetCharacterName(const std::string& name) {
        this->name =  name;
    }

    void SetCharacterHealth(int health) {
        this->health = health;
    }

    void SetCharacterScore(int score) {
        this->score = score;
    }

    // functie de adaugare a unui item in inventar
    void AddItemToInventory(std::shared_ptr<Item> item) {
        if (item) {
            inventory.AddElement(item);
        }
    }

    // definesc destructorul clasei
    ~Character() {
        inventory.ClearContainer();
    }

    // functie pentru a verifica daca inventarul playerului este gol
    bool CharacterInventoryEmpty() const {
        return inventory.ContainerIsEmpty();
    }

    // definesc o functie ce sterge un item selectat (dat ca parametru) din inventarul playerului
    void RemoveItemFromInventory(int index) {
        inventory.RemoveElement(index);
    }
};


//      Imi definesc clasa Puzzle fiind o clasa abstracta. Din aceasta o sa am doua clase derivate:
//          1. Puzzle de tip ghicitoare (RIDDLE): - se afiseaza ghicitoarea pe ecran, iar jucatorul trebuie sa
//                          introduca raspunsul corect pentru a castiga. Are doar 3 sanse sa introduca acel raspuns,
//                          daca nu, va pierde. Daca va ghici raspunsul corect, primeste puncte in plus la scor
//                          in functie de dificultatea acestuia.
//          2. Puzzle de tipul jocului Spanzuratoare (HANGMAN): - player-ul are doar 5 posibilitati de a ghici
//                          cate o litera corecta. daca ghiceste cuvantul corect, primeste puncte in plus la scor
//                          in functie de dificultatea acestuia.

// imi definesc ca prim pas o clasa speciala pentru exceptiile puzzle urilor
class PuzzleException : public std::exception {
protected:
    std::string message;

public:
    explicit PuzzleException(const std::string& message) : message(message) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// creez clase specifice tipurilor de exceptii
class InputException : public PuzzleException {
    public:
    explicit InputException(const std::string& message) : PuzzleException("[INPUT ERROR]: " + message) {}
};

class ItemException : public PuzzleException {
    public:
    explicit ItemException(const std::string& message) : PuzzleException("[ITEM ERROR]: " + message) {}
};

class MiniGameException : public PuzzleException {
    public:
    explicit MiniGameException(const std::string& message) : PuzzleException("[PUZZLE ERROR]: " + message) {}
};

// 1. BEHAVIORAL PATTERN (COMPORTAMENTAL)
// interfata Observer pentru a notifica cand se rezolva un puzzle

class Observer {
public:
    virtual void Update(const std::string& message, int pointsEarned) = 0;
    virtual ~Observer() = default;
};

// interfata Subject pentru obiectele care pot fi observate
class Subject {
private:
    std::vector<Observer*> observers;

public:
    virtual void Attach(Observer* observer) {
        observers.push_back(observer);
    }

    virtual void Detach(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    virtual void Notify(const std::string& message, int pointsEarned) {
        for (Observer* observer : observers) {
            observer->Update(message, pointsEarned);
        }
    }

    virtual ~Subject() = default;
};

// ScoreManager care implementează Observer
class ScoreManager : public Observer {
private:
    static ScoreManager* instance;
    std::vector<std::string> achievementLog;

    ScoreManager() = default;

public:
    static ScoreManager& GetInstance() {
        if (instance == nullptr) {
            instance = new ScoreManager();
        }
        return *instance;
    }

    static void DestroyInstance() {
        delete instance;
        instance = nullptr;
    }

    void Update(const std::string& message, int pointsEarned) override {
        std::string logEntry = "Achievement: " + message + " | Points: " + std::to_string(pointsEarned);
        achievementLog.push_back(logEntry);
        std::cout << "      [NOTIFICATION] " << logEntry << std::endl;
    }

    const std::vector<std::string>& GetAchievementLog() const {
        return achievementLog;
    }

    void DisplayAchievementLog() const {
        std::cout << "\n      === ACHIEVEMENT LOG ===" << std::endl;
        if (achievementLog.empty()) {
            std::cout << "      No achievements recorded yet." << std::endl;
        } else {
            for (const auto& entry : achievementLog) {
                std::cout << "      " << entry << std::endl;
            }
        }
        std::cout << "      ======================" << std::endl;
    }
};

// Inițializarea instanței ScoreManager
ScoreManager* ScoreManager::instance = nullptr;

// Definesc un DESIGN PATTERN STRUCTURAL -> de decor
// adaug efecte speciale pentru anumite iteme

// am o interfata comuna pentru toate itemele din joc
class GameItem {
public:
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual void Use(Character& character) = 0;
    virtual ~GameItem() = default;
};

// clasa de baza pentru decoratorii de iteme
class ItemDecorator : public GameItem {
protected:
    std::shared_ptr<GameItem> wrappedItem;
public:
    explicit ItemDecorator(std::shared_ptr<GameItem> item) : wrappedItem(item) {}

    std::string GetName() const override {
        return wrappedItem->GetName();
    }

    std::string GetDescription() const override {
        return wrappedItem->GetDescription();
    }

    void Use(Character& character) override {
        wrappedItem->Use(character);
    }
};

// decorator concret pentru iteme ce adauga experienta
class ExperienceBoostDecorator : public ItemDecorator {
private:
    int expBoost;

public:
    ExperienceBoostDecorator(std::shared_ptr<GameItem> item, int expBoost)
        : ItemDecorator(item), expBoost(expBoost) {}

    std::string GetName() const override {
        return "Experienced " + wrappedItem->GetName();
    }

    std::string GetDescription() const override {
        return wrappedItem->GetDescription() + " (+" + std::to_string(expBoost) + " EXP on use)";
    }

    void Use(Character& character) override {
        wrappedItem->Use(character);
        character.SetCharacterScore(character.GetCharacterScore() + expBoost);
        std::cout << "      [BOOST] Added " << expBoost << " EXP from experience boost!" << std::endl;
    }
};

// decorator concret pentru iteme care ofera efecte de durata
class DurationEffectDecorator : public ItemDecorator {
private:
    std::string effectDescription;
    int duration;

public:
    DurationEffectDecorator(std::shared_ptr<GameItem> item, const std::string& effectDesc, int duration)
        : ItemDecorator(item), effectDescription(effectDesc), duration(duration) {}

    std::string GetName() const override {
        return "Lasting " + wrappedItem->GetName();
    }

    std::string GetDescription() const override {
        return wrappedItem->GetDescription() + " (" + effectDescription +
               " for " + std::to_string(duration) + " turns)";
    }

    void Use(Character& character) override {
        wrappedItem->Use(character);
        std::cout << "      [EFFECT] " << effectDescription << " active for "
                  << duration << " turns!" << std::endl;
    }
};

// adaptez clasa Item existenta pentru a implementa interfata GameItem
class ItemAdapter : public GameItem {
private:
    std::shared_ptr<Item> originalItem;

public:
    explicit ItemAdapter(std::shared_ptr<Item> item) : originalItem(std::move(item)) {}

    std::string GetName() const override {
        // Se presupune că toate Item-urile au o metodă pentru a obține numele
        if (auto attackItem = std::dynamic_pointer_cast<AttackItem>(originalItem)) {
            return attackItem->GetAttackItemName();
        } else if (auto healItem = std::dynamic_pointer_cast<HealItem>(originalItem)) {
            return healItem->GetHealItemName();
        } else if (auto collectibleItem = std::dynamic_pointer_cast<CollectibleItem>(originalItem)) {
            return collectibleItem->GetCollectibleItemName();
        }
        return "Unknown Item";
    }

    std::string GetDescription() const override {
        // Se presupune că toate Item-urile au o metodă pentru descriere
        if (auto attackItem = std::dynamic_pointer_cast<AttackItem>(originalItem)) {
            return "Attack item with " + std::to_string(attackItem->GetAttackItemValue()) + " damage";
        } else if (auto healItem = std::dynamic_pointer_cast<HealItem>(originalItem)) {
            return "Heal item with " + std::to_string(healItem->GetHealItemValue()) + " healing";
        } else if (auto collectibleItem = std::dynamic_pointer_cast<CollectibleItem>(originalItem)) {
            return "Collectible with " + collectibleItem->GetCollectibleItemRarity() + " rarity";
        }
        return "Generic item description";
    }

    void Use(Character& character) override {
        // Implementarea utilizării itemului specific
        if (auto attackItem = std::dynamic_pointer_cast<AttackItem>(originalItem)) {
            std::cout << "      Using attack item: " << attackItem->GetAttackItemName() << std::endl;
            // Logica de folosire a unui item de atac
        } else if (auto healItem = std::dynamic_pointer_cast<HealItem>(originalItem)) {
            std::cout << "      Using heal item: " << healItem->GetHealItemName() << std::endl;
            character.SetCharacterHealth(character.GetCharacterHealth() + healItem->GetHealItemValue());
            std::cout << "      Healed for " << healItem->GetHealItemValue() << " points!" << std::endl;
        } else if (auto collectibleItem = std::dynamic_pointer_cast<CollectibleItem>(originalItem)) {
            std::cout << "      Admiring collectible: " << collectibleItem->GetCollectibleItemName() << std::endl;
        }
    }
};

class Puzzle {
protected:
    std::string puzzle_type;
    std::string difficulty;

public:
    // constructor fara parametrii
    Puzzle() {
        this->puzzle_type = "Puzzle";
        this->difficulty = "easy";
    }

    // constructor cu parametrii
    Puzzle(const std::string& puzzle_type, const std::string& difficulty) {
        this->puzzle_type = puzzle_type;
        this->difficulty = difficulty;
    }

    // constructorul de copiere
    Puzzle(Puzzle* puzzle) {
        this->puzzle_type = puzzle->puzzle_type;
        this->difficulty = puzzle->difficulty;
    }

    // supraincarcarea operatorului de atribuire
    Puzzle& operator=(const Puzzle& puzzle) {
        if (this != &puzzle) {
            this->puzzle_type = puzzle.puzzle_type;
            this->difficulty = puzzle.difficulty;
        }
        return *this;
    }

    // metoda publica de citire a datelor => functie pur virtuala
    virtual void ReadPuzzleInfo(std::istream& in) = 0;

    // metoda publica de afisare a datelor => functie pur virtuala
    virtual void DisplayPuzzleInfo(std::ostream& out) const = 0;

    // metoda de copiere a unui obiect de tip Puzzle => functie pur virtuala
    virtual Puzzle* Clone() const = 0;

    // supraincarcarea operatorului de scriere
    friend std::ostream& operator<<(std::ostream& out, const Puzzle& puzzle) {
        puzzle.DisplayPuzzleInfo(out);
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, Puzzle& puzzle) {
        puzzle.ReadPuzzleInfo(in);
        return in;
    }

    // getteri
    const std::string& GetPuzzleType() const {
        return this->puzzle_type;
    }
    const std::string& GetDifficulty() const {
        return this->difficulty;
    }

    // setteri
    void SetPuzzleType(const std::string& puzzle_type) {
        this->puzzle_type = puzzle_type;
    }
    void SetDifficulty(const std::string& difficulty) {
        this->difficulty = difficulty;
    }

    // destructor (virtual)
    virtual ~Puzzle() = default;
};

//     Definesc clasa derivata din clasa de baza/abstracta Puzzle. Este responsabila de jocurile de tip
//   ghicitori.

class Riddle : virtual public Puzzle {
protected:
    std::string rules;
    std::string description;
    std::string solution;
public:
    // constructor
    Riddle(const std::string& puzzle_type, const std::string& difficulty, const std::string& rules,
      const std::string& description, const std::string& solution)
          : Puzzle(puzzle_type, difficulty), rules(rules), description(description), solution(solution) {}

    // constructor de copiere
    Riddle(const Riddle& riddle)  : Puzzle(riddle) {
      this->puzzle_type = riddle.puzzle_type;
      this->difficulty = riddle.difficulty;
      this->rules = riddle.rules;
      this->description = riddle.description;
      this->solution = riddle.solution;
    }

    // supraincarcarea operatorului de atribuire
    Riddle& operator=(const Riddle& riddle) {
      if (this != &riddle) {
          this->puzzle_type = riddle.puzzle_type;
          this->difficulty = riddle.difficulty;
          this->rules = riddle.rules;
          this->description = riddle.description;
          this->solution = riddle.solution;
      }
      return *this;
    }

    // supraincarcarea operatorului de scriere din functia virtuala
    void DisplayPuzzleInfo(std::ostream& out) const override {
        out << "    Riddle type: " << this->puzzle_type << std::endl;
        out << "    Riddle difficulty: " << this->difficulty << std::endl;
        out << "    Riddle rules: " << this->rules << std::endl;
        out << "    Riddle description: " << this->description << std::endl;
        out << "    Riddle solution: " << this->solution << std::endl;
    }

    // supraincarcarea operatorului de citire prin functia virtuala
    void ReadPuzzleInfo(std::istream &in) override {
        std::cout << "  Enter the riddle type: ";
        in >> this->puzzle_type;
        std::cout << "  Enter the difficulty: ";
        in >> this->difficulty;
        std::cout << "  Enter the rules: ";
        in >> this->rules;
        std::cout << "  Enter the description: ";
        in >> this->description;
        std::cout << "  Enter the solution: ";
        in >> this->solution;
    }

    // definesc metoda de copiere a unui obiect de tip Riddle
    Puzzle* Clone() const override {
        return new Riddle(*this);
    }

    // getteri
    const std::string& GetRiddlewType() const {
        return this->puzzle_type;
    }
    const std::string& GetRiddleDifficulty() const {
        return this->difficulty;
    }
    const std::string& GetRiddleRules() const {
        return this->rules;
    }
    const std::string& GetRiddleDescription() const {
        return this->description;
    }
    const std::string& GetRiddleSolution() const {
        return this->solution;
    }

    // setteri
    void SetRiddleType(const std::string& puzzle_type) {
        this->puzzle_type = puzzle_type;
    }
    void SetRiddleDifficulty(const std::string& difficulty) {
        this->difficulty = difficulty;
    }
    void SetRiddleRules(const std::string& rules) {
        this->rules = rules;
    }
    void SetRiddleDescription(const std::string& description) {
        this->description = description;
    }
    void SetRiddleSolution(const std::string& solution) {
        this->solution = solution;
    }

    // destructor?
    ~Riddle() override = default;

    // implementez o metoda pentru rezolvarea unui puzzle
    bool CheckRiddleSolution() {
        static int max_attemps = 3;

        int attemps = 0;
        while (attemps < max_attemps) {
            std::string answer;
            std::cout << std::endl;
            std::cout << "      Your answer: (" << max_attemps - attemps << " remaining attempts): ";
            try {
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    throw InputException("Input stream is in a failed state.");
                }

                std::cin.ignore();
                std::getline(std::cin, answer);

                if (answer.empty()) {
                    throw InputException("      Empty answer provided.");
                }

                if (answer == this->solution) {
                    std::cout << "      Congratulations! You solved the puzzle correctly." << std::endl;
                    return true;
                }
                std::cout << "      Wrong answer! Please try again." << std::endl;
                attemps++;
            }
            catch (const InputException& e) {
                std::cerr << "      Error: " << e.what() << std::endl;
                std::cout << "      Please try again." << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "      Unexpected error: " << e.what() << std::endl;
                return false;
            }
        }
        std::cout << "      You lost! The correct solution was: " << this->solution << std::endl;
        return false;
    }
};

//      Definesc clasa derivata HangMan din clasa de baza/abstracta Puzzle. Aceasta este responsabila de jocurile de
//   tip spanzuratoarea.

class HangMan : virtual public Puzzle {
protected:
    std::string rules;
    std::string solution;

public:
    // constructor
    HangMan(const std::string& puzzle_type, const std::string& difficulty, const std::string& rules,
             const std::string& solution)
            : Puzzle(puzzle_type, difficulty), rules(rules), solution(solution) {}

    // constructor de copiere
    HangMan(const HangMan& hangman)  : Puzzle(hangman) {
        this->puzzle_type = hangman.puzzle_type;
        this->difficulty = hangman.difficulty;
        this->rules = hangman.rules;
        this->solution = hangman.solution;
    }

    // supraincarcarea operatorului de atribuire
    HangMan& operator=(const HangMan& hangman) {
        if (this != &hangman) {
            this->puzzle_type = hangman.puzzle_type;
            this->difficulty = hangman.difficulty;
            this->rules = hangman.rules;
            this->solution = hangman.solution;
        }
        return *this;
    }

    // supraincarcarea operatorului de scriere prin intermediul functiei virtuale
    void DisplayPuzzleInfo(std::ostream& out) const override {
        out << "    Hangman: " << this->puzzle_type << std::endl;
        out << "    Difficulty: " << this->difficulty << std::endl;
        out << "    Rules: " << this->rules << std::endl;
        out << "    Solution: " << this->solution << std::endl;
    }

    // supraincarcarea operatorului de citire prin intermediul functiei virtuale
    void ReadPuzzleInfo(std::istream &in) override {
        std::cout << "  Enter the riddle type: ";
        in >> this->puzzle_type;
        std::cout << "  Enter the difficulty: ";
        in >> this->difficulty;
        std::cout << "  Enter the rules: ";
        in >> this->rules;
        std::cout << "  Enter the solution: ";
        in >> this->solution;
    }

    // metoda de copiere a unui obiect
    Puzzle* Clone() const override {
        return new HangMan(*this);
    }

    // getteri
    const std::string& GetHangManType() const {
        return this->puzzle_type;
    }
    const std::string& GetHangManDifficulty() const {
        return this->difficulty;
    }
    const std::string& GetHangManRules() const {
        return this->rules;
    }
    const std::string& GetHangManSolution() const {
        return this->solution;
    }

    // setteri
    void SetHangManType(const std::string& hangman_type) {
        this->puzzle_type = hangman_type;
    }
    void SetHangManDifficulty(const std::string& difficulty) {
        this->difficulty = difficulty;
    }
    void SetHangManRules(const std::string& rules) {
        this->rules = rules;
    }
    void SetHangManSolution(const std::string& solution) {
        this->solution = solution;
    }

    // destructor?
    ~HangMan() override = default;
};

//  Definesc un DESIGN PATTERN -> CREATIONAL -> este o clasa abstracta prin care creez diferitele tipuri de puzzle
//  (riddle si hangman game).

// clasa abstracta pentru crearea de puzzle-uri

class PuzzleFactory {
public:
    virtual Puzzle* CreatePuzzle() const = 0;
    virtual ~PuzzleFactory() = default;
};

// clasa concreta pentru crearea de puzzle-uri de tip Riddle

class RiddleFactory : public PuzzleFactory {
private:
    std::string difficulty;
public:
    explicit RiddleFactory(const std::string& difficulty) : difficulty(difficulty) {}

    Puzzle* CreatePuzzle() const override {
        static std::string riddle_type =   "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |                       RIDDLE                            |";
        static std::string rule =          "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |       Rule: According to the description, you have      |\n"
                                           "      |          3 attempts to give the correct answer for      |\n"
                                           "      |          the following riddle.                          |";

        // folosesc un vector de riddle uri pentru fiecare dificultate
        std::vector<Riddle*> easyRiddles = {
            new Riddle(riddle_type, "Easy", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |       I charm with a smile, so handsome and kind,       |\n"
                                           "      |       But darkness and evil lurk in my mind.            |\n"
                                           "      |       I lure you close with a clever disguise,          |\n"
                                           "      |       Beware my true nature, behind these blue eyes.    |\n"
                                           "      |                    Who am I?                            |\n"
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "Ted Bundy"),
            new Riddle(riddle_type,  "Easy", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |   In a quiet apartment, my secrets reside,              |\n"
                                           "      |   A macabre collection I carefully hide.                |\n"
                                           "      |   I offer you drinks, but they are laced with despair,  |\n"
                                           "      |   A monster in silence, with victims to spare.          |\n"
                                           "      |             Who am I?                                   |\n "
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "Jeffrey Dahmer")
        };
        std::vector<Riddle*> mediumRiddles = {
            new Riddle(riddle_type, "Medium", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |     I wear a clown s face, with laughter and cheer,     |\n"
                                           "      |     But beneath the paint, your worst fears appear.     |\n"
                                           "      |     In my crawl space, the truth lies below,            |\n"
                                           "      |     A killer in costume, with horrors to show.          |\n"
                                           "      |                 Who am I?                               |\n"
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "John Wayne Gacy"),
            new Riddle(riddle_type, "Medium", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |        Letters and ciphers, I send with delight,        |\n"
                                           "      |        A shadowy figure who stalks in the night.        |\n"
                                           "      |        My identity s hidden, a mystery still,           |\n"
                                           "      |        A puzzle unsolved, with victims to chill.        |\n"
                                           "      |                      Who am I?                          |\n"
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "Zodiac Killer")
        };
        std::vector<Riddle*> hardRiddles = {
            new Riddle(riddle_type, "Hard", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |        In Whitechapel s fog, I vanish from sight,       |\n"
                                           "      |        A butcher of women, I strike in the night.       |\n"
                                           "      |        My letters and taunts, a mystery still,          |\n"
                                           "      |        A name that evokes both terror and chill.        |\n"
                                           "      |                       Who am I?                         |\n"
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "Jack the Ripper"),
            new Riddle(riddle_type, "Hard", rule,
                               "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                                           "      |      I creep in the night, with a pentagram s sign,     |\n"
                                           "      |      A devilish figure, your nightmares align.          |\n"
                                           "      |      Through windows I enter, with terror I bring,      |\n"
                                           "      |      A killer who worships the dark and its sting.      |\n"
                                           "      |                    Who am I?                            |\n"
                                           "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                                           "Richard Ramirez")
        };

        // selectez riddle-uri pe baza de dificultate
        std::vector<Riddle*>* selectedRiddle = nullptr;

        if (difficulty == "Easy") {
            selectedRiddle = &easyRiddles;
        } else if (difficulty == "Medium") {
            selectedRiddle = &mediumRiddles;
        } else if (difficulty == "Hard") {
            selectedRiddle = &hardRiddles;
        } else {
            // daca nu am o dificultate selectata atunci ea este default easy
            selectedRiddle = &easyRiddles;
        }

        // aleg un riddle aleator din categoria selectata
        if (!selectedRiddle->empty()) {
            srand(static_cast<unsigned int>(time(nullptr)));
            int randomIndex = rand() % selectedRiddle->size();
            return (*selectedRiddle)[randomIndex]->Clone();
        }

        // returnez un riddle default daca nu am altceva
        return new Riddle(riddle_type, "Easy", rule,
                         "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +\n"
                         "      |       Default riddle when no others are available       |\n"
                         "      + - - - - - - - - - - - - - - - - - - - - - - - - - - - - +",
                         "Default");
    }
};

class HangManFactory : public PuzzleFactory {
private:
    std::string difficulty;
public:
    explicit HangManFactory(const std::string& difficulty) : difficulty(difficulty) {}

    Puzzle* CreatePuzzle() const override {
        static std::string rule = "     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n"
                                  "     *  Rule: You have to guess the word by suggesting letters from the alphabet.  *\n"
                                  "     *  Each time a letter is chosen that is not in the word, part of the little   *\n"
                                  "     *  man on the gallows is colored. You have 6 attempts to fail.                *\n"
                                  "     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n";
        static std::string hangman_type = "Hangman";
        std::vector<HangMan*> easyHangMans = {
            new HangMan(hangman_type, "Easy", rule, "nun"),
            new HangMan(hangman_type, "Easy", rule, "clown"),
            new HangMan(hangman_type, "Easy", rule, "ghost"),
            new HangMan(hangman_type, "Easy", rule, "knife")
        };
        std::vector<HangMan*> mediumHangMans = {
            new HangMan(hangman_type, "Medium", rule, "fireball"),
            new HangMan(hangman_type, "Medium", rule, "criminal"),
            new HangMan(hangman_type, "Medium", rule, "annabelle"),
            new HangMan(hangman_type, "Medium", rule, "ritual")
        };
        std::vector<HangMan*> hardHangMans = {
            new HangMan(hangman_type, "Hard", rule, "slenderman"),
            new HangMan(hangman_type, "Hard", rule, "exorcism"),
            new HangMan(hangman_type, "Hard", rule, "nightmare")
        };

        // selectez jocul de hangman bazat pe dificultate
        std::vector<HangMan*>* selectedHangMan = nullptr;

        if (difficulty == "Easy") {
            selectedHangMan = &easyHangMans;
        }
        else if (difficulty == "Medium") {
            selectedHangMan = &mediumHangMans;
        }
        else if (difficulty == "Hard") {
            selectedHangMan = &hardHangMans;
        }
        else {
            // daca dificultatea nu este selectata atunci este default easy
            selectedHangMan = &easyHangMans;
        }

        // aleg un joc de hangman aleator din categoria selectata
        if (!selectedHangMan->empty()) {
            srand(static_cast<unsigned int>(time(nullptr)));
            int randomIndex = rand() % selectedHangMan->size();
            return (*selectedHangMan)[randomIndex]->Clone();
        }

        // returnnes un hangman default daca nu am altceva
        return new HangMan(rule, "Easy", rule, "default");
    }
};

// implementez o clasa Utilities -> este responsabila cu encapsularea functiilor globale indepentente
class Utilities {
private:
    // constructor privat pt a preveni instantierea
    Utilities() = default;

    // instantiez un Singleton
    static Utilities *instance;
    public:
    // metoda pt a instantia Singleton ul
    static Utilities& GetInstance() {
        if (instance == nullptr) {
            instance = new Utilities();
        }
        return *instance;
    }

    // metoda pentru a elibera memoria
    static void DestroyInstance() {
        delete instance;
        instance = nullptr;
    }

    // functiile incapsulate
    void BattleCopy(Character& player, Enemy& enemy);
    void BattleCH_SECopy(Character& player, SpecialEnemy& special_enemy);
    void StartRiddleGameCopy(Character& player);
    void StartHangManGameCopy(Character& player);
};

// instaltiez pointer ul la singleton
Utilities* Utilities::instance = nullptr;

// incapsulez acum functiile globale care apeleaza metodele din clasa Utilities
// vreau sa folosesc acelasi mod de apelare
inline void Battle(Character& player, Enemy& enemy) {
    Utilities::GetInstance().BattleCopy(player, enemy);
}

inline void BattleCH_SE(Character& player, SpecialEnemy& special_enemy) {
    Utilities::GetInstance().BattleCH_SECopy(player, special_enemy);
}

inline void StartRiddleGame(Character& player) {
    Utilities::GetInstance().StartRiddleGameCopy(player);
}

inline void StartHangManGame(Character& player) {
    Utilities::GetInstance().StartHangManGameCopy(player);
}

void Utilities::BattleCopy(Character &player, Enemy &enemy) {
    try {
        std::cout << std::endl;
        std::cout << "      === BATTLE STARTS: " << player.GetCharacterName() << " vs " << enemy.GetEnemyName() << " ===" << std::endl;
        std::cout << "      Enemy " << enemy.GetEnemyName() << " (Health: " << enemy.GetEnemyHealth()
                  << ", Attack: " << enemy.GetEnemyAttackDamage() << ")" << std::endl;

        // lupta continua pana unul este invins
        bool playerTurn = true;  // incepe player ul

        while (player.GetCharacterHealth() > 0 && enemy.GetEnemyHealth() > 0) {
            if (playerTurn) {
                // tura player ului
                std::cout << "      === " << player.GetCharacterName() << "'s turn ===" << std::endl;
                std::cout << "      Your Health: " << player.GetCharacterHealth() << std::endl;
                std::cout << "      Enemy Health: " << enemy.GetEnemyHealth() << std::endl;
                std::cout << std::endl;
                std::cout << "      Choose your action:" << std::endl;
                std::cout << "      1. Basic Attack (damage: 10)" << std::endl;
                std::cout << "      2. Use Item from Inventory" << std::endl;

                int choice;
                std::cout << "      Enter your choice (1-2): ";
                std::cin >> choice;

                // curat buffer ul de intrare ca sa n am erori
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (choice == 1) {
                    // atac basic
                    int damage = 10;
                    enemy.SetEnemyHealth(enemy.GetEnemyHealth() - damage);
                    std::cout << "      You attacked " << enemy.GetEnemyName() << " for " << damage << " damage!" << std::endl;
                }
                else if (choice == 2) {
                    // folosesc item din inventar
                    if (player.CharacterInventoryEmpty()) {
                        std::cout << "      Your inventory is empty!" << std::endl;
                    }
                    else {
                        std::cout << std::endl;
                        std::cout << "      Your inventory:" << std::endl;
                        for (int i = 0; i < player.GetInventorySize(); i++) {
                            std::cout << i + 1 << ". ";
                            auto item = player.GetCharacterInventoryItem(i);
                            if (item) {
                                item->DisplayItemInfo(std::cout);
                            } else {
                                std::cout << "      Invalid item" << std::endl;
                            }
                        }

                        // selectarea unui item
                        int itemIndex;
                        std::cout << "      Choose an item (1-" << player.GetInventorySize() << "): ";
                        std::cin >> itemIndex;

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        itemIndex--;

                        if (itemIndex >= 0 && itemIndex < player.GetInventorySize()) {
                            std::shared_ptr<Item> selectedItem = player.GetCharacterInventoryItem(itemIndex);

                            if (!selectedItem) {
                                std::cout << "      Error: Selected item is null!" << std::endl;
                                continue;
                            }

                            // verific tipul itemului
                            std::shared_ptr<AttackItem> attackItem = std::dynamic_pointer_cast<AttackItem>(selectedItem);
                            if (attackItem) {
                                int damage = attackItem->GetAttackItemValue();
                                enemy.SetEnemyHealth(enemy.GetEnemyHealth() - damage);
                                std::cout << "      You used " << attackItem->GetItemName() << " and dealt "
                                          << damage << " damage to " << enemy.GetEnemyName() << "!" << std::endl;
                            }
                            else {
                                std::shared_ptr<HealItem> healItem = std::dynamic_pointer_cast<HealItem>(selectedItem);
                                if (healItem) {
                                    int healAmount = healItem->GetHealItemValue();
                                    player.SetCharacterHealth(player.GetCharacterHealth() + healAmount);
                                    std::cout << "      You used " << healItem->GetItemName() << " and recovered "
                                              << healAmount << " health points!" << std::endl;
                                }
                                else {
                                    std::shared_ptr<CollectibleItem> collectibleItem = std::dynamic_pointer_cast<CollectibleItem>(selectedItem);
                                    if (collectibleItem) {
                                        std::cout << "      You can't use " << collectibleItem->GetItemName()
                                                  << " in battle. It's a collectible item." << std::endl;
                                    }
                                    else {
                                        std::cout << "      Unknown item type!" << std::endl;
                                    }
                                }
                            }
                        }
                        else {
                            std::cout << "      Invalid item selection!" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "      Invalid choice. You missed your turn!" << std::endl;
                }
            }
            else {
                // tura inamicului
                std::cout << std::endl;
                std::cout << "      === " << enemy.GetEnemyName() << "'s turn ===" << std::endl;

                // enemy ul ataca playerul
                int damage = enemy.GetEnemyAttackDamage();
                player.SetCharacterHealth(player.GetCharacterHealth() - damage);
                std::cout << "      " << enemy.GetEnemyName() << " attacked you for " << damage << " damage!" << std::endl;
            }

            // schimb tura
            playerTurn = !playerTurn;
        }

        if (player.GetCharacterHealth() <= 0) {
            std::cout << std::endl;
            std::cout << "      === BATTLE ENDED: " << enemy.GetEnemyName() << " WON ===" << std::endl;
            std::cout << "      You were defeated by " << enemy.GetEnemyName() << "!" << std::endl;

            // reduc din scorul playerului
            player.SetCharacterScore(player.GetCharacterScore() - 10);
        }
        else {
            std::cout << std::endl;
            std::cout << "      === BATTLE ENDED: " << player.GetCharacterName() << " WON ===" << std::endl;
            std::cout << "      You defeated " << enemy.GetEnemyName() << "!" << std::endl;

            // maresc scorul playerului daca acesta a castigat
            int scoreGain = 20 * enemy.GetEnemyLevel();
            player.SetCharacterScore(player.GetCharacterScore() + scoreGain);
            std::cout << "      You gained " << scoreGain << " points!" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "      [ERROR] Exception caught during battle: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "      [ERROR] Unknown exception caught during battle!" << std::endl;
    }
}

void Utilities::BattleCH_SECopy(Character &player, SpecialEnemy &special_enemy) {
    try {
        std::cout << std::endl;
        std::cout << "      === BATTLE STARTS: " << player.GetCharacterName() << " vs " << special_enemy.GetSpecialEnemyName() << " ===" << std::endl;
        std::cout << "      Special Enemy " << special_enemy.GetSpecialEnemyName()
                  << " (Health: " << special_enemy.GetSpecialEnemyHealth()
                  << ", Attack: " << special_enemy.GetSpecialEnemyAttackDamage()
                  << ", Weapon: " << special_enemy.GetSpecialEnemyItemName()
                  << " with " << special_enemy.GetSpecialEnemyAttackItemValue() << " damage)" << std::endl;

        // lupta continua pana unul dintre ei este invins
        bool playerTurn = true;  // player ul incepe mereu

        while (player.GetCharacterHealth() > 0 && special_enemy.GetSpecialEnemyHealth() > 0) {
            if (playerTurn) {
                // Player's turn
                std::cout << std::endl;
                std::cout << "      === " << player.GetCharacterName() << "'s turn ===" << std::endl;
                std::cout << "      Your Health: " << player.GetCharacterHealth() << std::endl;
                std::cout << "      Enemy Health: " << special_enemy.GetSpecialEnemyHealth() << std::endl;
                std::cout << std::endl;
                std::cout << "      Choose your action:" << std::endl;
                std::cout << "      1. Basic Attack (damage: 10)" << std::endl;
                std::cout << "      2. Use Item from Inventory" << std::endl;

                int choice;
                std::cout << "      Enter your choice (1-2): ";
                std::cin >> choice;

                // curat buffer ul ca sa n am erori
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (choice == 1) {
                    // atac basic prestabilit
                    int damage = 10;
                    special_enemy.SetSpecialEnemyHealth(special_enemy.GetSpecialEnemyHealth() - damage);
                    std::cout << "      You attacked " << special_enemy.GetSpecialEnemyName() << " for " << damage << " damage!" << std::endl;
                }
                else if (choice == 2) {
                    // utilizarea unui item din inventar
                    if (player.CharacterInventoryEmpty()) {
                        std::cout << "      Your inventory is empty!" << std::endl;
                    }
                    else {
                        std::cout << std::endl;
                        std::cout << "      Your inventory:" << std::endl;
                        for (int i = 0; i < player.GetInventorySize(); i++) {
                            std::cout << i + 1 << ". ";
                            auto item = player.GetCharacterInventoryItem(i);
                            if (item) {
                                item->DisplayItemInfo(std::cout);
                            } else {
                                std::cout << "      Invalid item" << std::endl;
                            }
                        }

                        // selectez un item
                        int itemIndex;
                        std::cout << "      Choose an item (1-" << player.GetInventorySize() << "): ";
                        std::cin >> itemIndex;

                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        itemIndex--;

                        if (itemIndex >= 0 && itemIndex < player.GetInventorySize()) {
                            std::shared_ptr<Item> selectedItem = player.GetCharacterInventoryItem(itemIndex);

                            if (!selectedItem) {
                                std::cout << "      Error: Selected item is null!" << std::endl;
                                continue;
                            }

                            // verific tipul itemului
                            std::shared_ptr<AttackItem> attackItem = std::dynamic_pointer_cast<AttackItem>(selectedItem);
                            if (attackItem) {
                                int damage = attackItem->GetAttackItemValue();
                                special_enemy.SetSpecialEnemyHealth(special_enemy.GetSpecialEnemyHealth() - damage);
                                std::cout << "      You used " << attackItem->GetItemName() << " and dealt "
                                          << damage << " damage to " << special_enemy.GetSpecialEnemyName() << "!" << std::endl;
                            }
                            else {
                                std::shared_ptr<HealItem> healItem = std::dynamic_pointer_cast<HealItem>(selectedItem);
                                if (healItem) {
                                    int healAmount = healItem->GetHealItemValue();
                                    player.SetCharacterHealth(player.GetCharacterHealth() + healAmount);
                                    std::cout << "      You used " << healItem->GetItemName() << " and recovered "
                                              << healAmount << " health points!" << std::endl;
                                }
                                else {
                                    std::shared_ptr<CollectibleItem> collectibleItem = std::dynamic_pointer_cast<CollectibleItem>(selectedItem);
                                    if (collectibleItem) {
                                        std::cout << "      You can't use " << collectibleItem->GetItemName()
                                                  << " in battle. It's a collectible item." << std::endl;
                                    }
                                    else {
                                        std::cout << "      Unknown item type!" << std::endl;
                                    }
                                }
                            }
                        }
                        else {
                            std::cout << "      Invalid item selection!" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "      Invalid choice. You missed your turn!" << std::endl;
                }
            }
            else {
                // tura enemy ului
                std::cout << std::endl;
                std::cout << "      === " << special_enemy.GetSpecialEnemyName() << "'s turn ===" << std::endl;

                bool useWeapon = (rand() % 100) < 30; // 30% sanse sa foloseasca si itemul de atac pe care il are

                if (useWeapon) {
                    // special_enemy foloseste si item ul de atac
                    int damage = special_enemy.GetSpecialEnemyAttackItemValue();
                    player.SetCharacterHealth(player.GetCharacterHealth() - damage);
                    std::cout << "      " << special_enemy.GetSpecialEnemyName() << " used its "
                              << special_enemy.GetSpecialEnemyItemName() << " and dealt "
                              << damage << " damage to you!" << std::endl;
                } else {
                    // special_enemy doar da un damage basic
                    int damage = special_enemy.GetSpecialEnemyAttackDamage();
                    player.SetCharacterHealth(player.GetCharacterHealth() - damage);
                    std::cout << "      " << special_enemy.GetSpecialEnemyName() << " attacked you for "
                              << damage << " damage!" << std::endl;
                }
            }

            // se schimba tura
            playerTurn = !playerTurn;
        }

        if (player.GetCharacterHealth() <= 0) {
            std::cout << std::endl;
            std::cout << "      === BATTLE ENDED: " << special_enemy.GetSpecialEnemyName() << " WON ===" << std::endl;
            std::cout << "      You were defeated by " << special_enemy.GetSpecialEnemyName() << "!" << std::endl;

            // reduc scorul playerului
            player.SetCharacterScore(player.GetCharacterScore() - 15);
        }
        else {
            std::cout << std::endl;
            std::cout << "      === BATTLE ENDED: " << player.GetCharacterName() << " WON ===" << std::endl;
            std::cout << "      You defeated " << special_enemy.GetSpecialEnemyName() << "!" << std::endl;

            int scoreGain = 30 * special_enemy.GetSpecialEnemyLevel();
            player.SetCharacterScore(player.GetCharacterScore() + scoreGain);
            std::cout << "      You gained " << scoreGain << " points!" << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "      [ERROR] Exception caught during special battle: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "      [ERROR] Unknown exception caught during special battle!" << std::endl;
    }
}

void Utilities::StartRiddleGameCopy(Character &player) {
    try {
        // instantiez observer ul
        ScoreManager& score_manager = ScoreManager::GetInstance();

        // declar player ul ca subject
        Subject playerSubject;
        playerSubject.Attach(&score_manager);

        // generez un puzzle aleatoriu
        srand(static_cast<unsigned int>(time(nullptr)));

        // selectez dificultatea aleatoare
        std::string difficulties[] = {"Easy", "Medium", "Hard"};
        int difficultyIndex = rand() % 3;
        std::string selectedDifficulty = difficulties[difficultyIndex];

        // folosesc factory pentru a crea puzzle-ul
        RiddleFactory riddleFactory(selectedDifficulty);
        Puzzle* puzzle = riddleFactory.CreatePuzzle();

        // DOWNCASTING pentru a accesa metodele specifice de la Riddle
        Riddle* riddle = dynamic_cast<Riddle*>(puzzle);

        if (!riddle) {
            throw MiniGameException("       Failed to create a valid riddle puzzle.");
        }

        AttackItem items_attack[] = {
            AttackItem("Bloodthorn Dagger", "Dangerous", 40),
            AttackItem("Stormbringer Gauntlet", "Dangerous", 30),
            AttackItem("Venomfang Blade", "Dangerous", 50),
            AttackItem("Soulfire Orb", "Dangerous", 45),
            AttackItem("Doomhammer", "Dangerous", 20),
            AttackItem("Frostbite Arrow", "Dangerous", 35),
            AttackItem("Infernal Granade", "Dangerous", 60),
            AttackItem("Necrotic Chains", "Dangerous", 50),
            AttackItem("Deathspike Bolts", "Dangerous", 45),
            AttackItem("Hellhound Claws", "Dangerous", 50),
            AttackItem("Ebonflame Crossbow", "Dangerous", 40),
            AttackItem("Bloodletter Katar", "Dangerous", 30)
        };

        HealItem items_heal[] = {
            HealItem("Elixir of Eternal Vitality", "Mystic", 100),
            HealItem("Moonflower Essence", "Mystic", 30),
            HealItem("Phoenix Down", "Mystic", 20),
            HealItem("Angelic Tear", "Mystic", 35),
            HealItem("Celestial Dewdrop", "Mystic", 20),
            HealItem("Sacred Lotus Petal", "Mystic", 30),
            HealItem("Radiant Sapphire Medallion", "Mystic", 20),
            HealItem("Golden Nectar of the Fae", "Mystic", 30),
            HealItem("Mystic Honeycomb", "Mystic", 10)
        };

        CollectibleItem items_collectible[] = {
            CollectibleItem("Runnestone of the Forgotten", "Magical", "Unique"),
            CollectibleItem("Astral Key", "Magical", "Unique"),
            CollectibleItem("Clockwork Gear of Passage", "Magical", "Unique"),
            CollectibleItem("Eye of the Guardian", "Magical", "Unique")
        };

        std::cout << "      Solve the riddle (" << riddle->GetRiddleDifficulty() << "):" << std::endl;
        std::cout << riddle->GetRiddlewType() << std::endl;
        std::cout << riddle->GetRiddleRules() << std::endl;
        std::cout << riddle->GetRiddleDescription() << std::endl;

        bool riddleSolved = false;
        try {
            riddleSolved = riddle->CheckRiddleSolution();
        }
        catch (const PuzzleException& e) {
            std::cerr << "      [RIDDLE ERROR]: " << e.what() << std::endl;
            // eliberez memoria
            delete puzzle;
            return;
        }

        int pointsEarned = 0;
        if (riddleSolved) {
            // daca solutia este corecta, player ul primeste un item corect si puncte in plus in functie de dificultate
            std::cout << std::endl;

            // punctaj in functie de dificultate
            if (riddle->GetRiddleDifficulty() == "Easy") {
                pointsEarned = 20;
                std::cout << "      As a reward, you received a surprise gift item and 20 extra points to your score!"
                        << std::endl;
                player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
            }
            else if (riddle->GetRiddleDifficulty() == "Medium") {
                pointsEarned = 30;
                std::cout << "      As a reward, you received a surprise gift item and 30 extra points to your score!"
                        << std::endl;
                player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
            }
            else if (riddle->GetRiddleDifficulty() == "Hard") {
                pointsEarned = 50;
                std::cout << "      As a reward, you received a surprise gift item and 40 extra points to your score!"
                        << std::endl;
                player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
            }

            // notifix observer ul despre rezolvarea puzzle ului
            playerSubject.Notify("      Riddle solved: " + riddle->GetRiddleDifficulty(), pointsEarned);

            std::cout << "      Your score is now: " << player.GetCharacterScore() << std::endl;
            std::cout << std::endl;
            std::cout << "      What type of item would you like to receive?" << std::endl;
            int choice;
            std::cout << "      1. ATTACK ITEM" << std::endl;
            std::cout << "      2. HEAL ITEM" << std::endl;
            std::cout << "      3. COLLECTIBLE ITEM" << std::endl;
            std::cout << "      Your choice: ";

            try {
                if (!(std::cin >> choice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    throw InputException("Invalid input. Please enter a number.");
                }

                if (choice < 1 || choice > 3) {
                    throw InputException("Choice must be between 1 and 3.");
                }

                if (choice == 1) {
                    // player ul a ales un item de atac aleatoriu
                    static int itemsAttackCount = std::size(items_attack);
                    static int randomAttack = rand() % itemsAttackCount;

                    // folosesc Decorator Pattern pentru a imbunatati itemul
                    auto baseItem = std::make_shared<AttackItem>(items_attack[randomAttack]);
                    auto adaptedItem = std::make_shared<ItemAdapter>(baseItem);

                    // adaug un decorator daca dificultatea este mai mare
                    if (riddle->GetRiddleDifficulty() == "Hard") {
                        auto decoratedItem = std::make_shared<ExperienceBoostDecorator>(adaptedItem, 10);
                        std::cout << "      You got special item: " << decoratedItem->GetName() <<
                            " - " << decoratedItem->GetDescription() << std::endl;
                    }
                    else {
                        player.AddItemToInventory(baseItem);
                        std::cout << "      You got " << items_attack[randomAttack].GetAttackItemName() << " with " <<
                            items_attack[randomAttack].GetAttackItemValue() << " damage." << std::endl;
                    }
                }
                else if (choice == 2) {
                    // player ul a ales un item de heal aleatoriu
                    static int itemsHealCount = std::size(items_heal);
                    static int randomHeal = rand() % itemsHealCount;

                    // folosesc Decorator Pattern pentru iteme de heal speciali
                    if (riddle->GetRiddleDifficulty() == "Hard" || riddle->GetRiddleDifficulty() == "Medium") {
                        auto baseItem = std::make_shared<HealItem>(items_heal[randomHeal]);
                        auto adaptedItem = std::make_shared<ItemAdapter>(baseItem);
                        auto decoratedItem = std::make_shared<DurationEffectDecorator>(
                            adaptedItem, "Regeneration", 3
                        );

                        std::cout << "      You got a special item: " << decoratedItem->GetName()
                                << " - " << decoratedItem->GetDescription() << std::endl;

                        // folosesc itemul adaptat direct
                        decoratedItem->Use(player);
                    } else {
                        player.AddItemToInventory(std::make_shared<HealItem>(items_heal[randomHeal]));
                        std::cout << "      You got " << items_heal[randomHeal].GetHealItemName() << " with " <<
                            items_heal[randomHeal].GetHealItemValue() << " health." << std::endl;
                    }
                }
                else if (choice == 3) {
                    // player ul a ales un item de collectible aleatoriu
                    static int itemsCollectibleCount = std::size(items_collectible);
                    static int randomCollectible = rand() % itemsCollectibleCount;

                    player.AddItemToInventory(std::make_shared<CollectibleItem>(items_collectible[randomCollectible]));
                    std::cout << "      You got " << items_collectible[randomCollectible].GetCollectibleItemName() << " with "
                       << items_collectible[randomCollectible].GetCollectibleItemRarity() << " rarity." << std::endl;
                }
            }
            catch (const InputException& e) {
                std::cerr << "      [INPUT ERROR]: " << e.what() << std::endl;
                std::cout << "      You didn t receive an item due to invalid input." << std::endl;
            }
            catch (const ItemException& e) {
                std::cerr << "      [ITEM ERROR]: " << e.what() << std::endl;
                std::cout << "      You didn't receive an item due to inventory issues." << std::endl;
            }
        }
        else {
            std::cout << "      The solution was incorrect. You did not receive a gift item." << std::endl;
            std::cout << "      Because you lost, 10 points were deducted from your score." << std::endl;
            std::cout << std::endl;
            pointsEarned = -10;
            player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);

            // notific observer ul despre esecul rezolvarii puzzle ului
            playerSubject.Notify("      Riddle unsolved: " + riddle->GetRiddleDifficulty(), pointsEarned);
            std::cout << "      Your score is now: " << player.GetCharacterScore() << std::endl;
            std::cout << std::endl;
        }
        // afisez jurnalul
        score_manager.DisplayAchievementLog();

        // eliberez memoria alocata
        delete puzzle;
    }
    catch (const MiniGameException& e) {
        std::cerr << "      [GAME ERROR]: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "      Critical game error: " << e.what() << std::endl;
        std::cout << "      The Riddle Game could t be started." << std::endl;
    }
}

void Utilities::StartHangManGameCopy(Character &player) {
    try {
        // initializez observer ul
        ScoreManager& scoreManager = ScoreManager::GetInstance();

        // declar player ul ca subject
        Subject playerSubject;
        playerSubject.Attach(&scoreManager);

        // pentru a genera un puzzle aleatoriu
        srand(static_cast<unsigned int>(time(nullptr)));

        // selectez dificultatea aleatoare
        std::string difficulties[] = {"Easy", "Medium", "Hard"};
        int difficultyIndex = rand() % 3;
        std::string selectedDifficulty = difficulties[difficultyIndex];

        // folosesc factory pentru a crea puzzle uri
        HangManFactory hangmanFactory(selectedDifficulty);
        Puzzle* puzzle = hangmanFactory.CreatePuzzle();

        // DOWNCASTING pentru a accesa metodele specifice HangMan
        HangMan* hangman = dynamic_cast<HangMan*>(puzzle);

        if (!hangman) {
            throw MiniGameException("Failed to create a valid hangman puzzle.");
        }

        // implementez logica jocului

        // etapele desenului
        static const std::vector<std::string> hangman_types = {
            R"(
                + - - - - -
                |         |
                |
                |
                |
                |
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |
                |
                |
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |         |
                |         |
                |
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |        /|
                |         |
                |
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |        /|\
                |         |
                |
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |        /|\
                |         |
                |        /
                |
                |
                |
            - - - - - - - - - -
        )",
            R"(
                + - - - - -
                |         |
                |         O
                |        /|\
                |         |
                |        / \
                |
                |
                |
            - - - - - - - - - -
        )",
        };

        if (hangman_types.empty()) {
            throw MiniGameException("HangMan drawings are not defined.");
        }

        // afisare
        std::cout << "              =================== HANGMAN GAME =======================" << std::endl;

        std::cout << hangman->GetHangManRules() << std::endl;

        if(hangman_types.size() <= 0) {
            throw MiniGameException("No HangMan drawings available.");
        }

        std::cout << hangman_types[0] << std::endl;
        std::cout << std::endl;

        // cuvantul
        static std::string word = hangman->GetHangManSolution();

        if (word.empty()) {
            throw MiniGameException("Selected HangMan word is empty.");
        }

        std::string guessedWord(word.length(), '_');


        std::cout << "          You have to guess the word formed by the following letters: " << std::endl;
        std::cout << "          Word difficulty: " << hangman->GetHangManDifficulty() << std::endl;
        std::cout << std::endl;

        std::vector<char> triedLetters;
        int mistakes = 0;
        static int maxMistakes = 6;

        while (mistakes < maxMistakes && guessedWord != word) {
            std::cout << "      Word: ";
            for (char c : guessedWord) {
                std::cout << c << " ";
            }
            std::cout << "      Tried Letters: ";
            for (char c : triedLetters) {
                std::cout << c << " ";
            }

            std::cout << std::endl;
            std::cout << "      Choose a letter: ";
            char letter;

            try {
                if (!(std::cin >> letter)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    throw InputException("Invalid input. Please enter a single letter.");
                }
                // o fac mica
                letter = std::tolower(letter);

                if (!std::isalpha(letter)) {
                    throw InputException("Input must be a letter from the alphabet.");
                }

                // verific daca a fost deja incercata
                if (std::ranges::find(triedLetters, letter) != triedLetters.end()) {
                    std::cout << "      You ve already tried the letter '" << letter << "'." << std::endl;
                    std::cout << "      Try another letter." << std::endl;
                } else {
                    triedLetters.push_back(letter);
                }

                // verific daca litera exista in cuvant
                bool found = false;
                for (int i = 0; i < word.length(); i++) {
                    if (word[i] == letter) {
                        guessedWord[i] = letter;
                        found = true;
                    }
                }

                if (!found) {
                    mistakes++;
                    std::cout << "      The letter does not exist in the word. You have " << mistakes << " mistakes."
                    << std::endl;
                }

                std::cout << hangman_types[mistakes] << std::endl;
            }
            catch (const InputException &e) {
                std::cerr << "      [ERROR HANGMAN GAME]: " << e.what() << std::endl;
                std::cout << "      Please try again!" << std::endl;
            }
        }

        int pointsEarned = 0;
        // final de joc
        if (guessedWord == word) {
            std::cout << "      Congratulations! You guessed the word!" << std::endl;
            std::cout << "      + As a reward, you get extra points on your score." << std::endl;

            try {
                if (hangman->GetHangManDifficulty() == "Easy") {
                    pointsEarned = 40;
                    player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
                    std::cout << "      Because the game was " << hangman->GetHangManDifficulty() << ", you will"
                    "  recive 40 extra points!" << std::endl;
                }
                else if (hangman->GetHangManDifficulty() == "Medium") {
                    pointsEarned = 50;
                    player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
                    std::cout << "      Because the game was " << hangman->GetHangManDifficulty() << ", you will"
                    "recive 50 extra points!" << std::endl;
                }
                else if (hangman->GetHangManDifficulty() == "Hard") {
                    pointsEarned = 60;
                    player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);
                    std::cout << "      Because the game was " << hangman->GetHangManDifficulty() << ", you will"
                    "recive 50 extra points!" << std::endl;
                }
                else {
                    throw PuzzleException("Unknown difficulty level: " + hangman->GetHangManDifficulty());
                }

                // notific observer ul despre rezolvarea puzzle ului
                playerSubject.Notify("      Hangman solved: " + hangman->GetHangManDifficulty(), pointsEarned);

                std::cout << "      Your current score: " << player.GetCharacterScore() << std::endl;
            }
            catch (const PuzzleException &e) {
                std::cerr << "      [ERROR HANGMAN GAME]: " << e.what() << std::endl;
                std::cout << "      You receive the default reward of 30 points." << std::endl;
                player.SetCharacterScore(player.GetCharacterScore() + 30);
            }
        }
        else {
            std::cout << "      The hangman puzzle was not solved. You did not receive a gift item." << std::endl;
            std::cout << "      Because you lost, 15 points were deducted from your score." << std::endl;
            std::cout << std::endl;
            pointsEarned = -15;
            player.SetCharacterScore(player.GetCharacterScore() + pointsEarned);

            // notific observer ul despre esuarea jocului de hangman
            playerSubject.Notify("      Hangman failed: " + hangman->GetHangManDifficulty(), pointsEarned);

            std::cout << "      Your score is now: " << player.GetCharacterScore() << std::endl;
        }
        // afisez jurnalul
        scoreManager.DisplayAchievementLog();

        // eliberez memoria
        delete puzzle;
    } catch (const std::exception &e) {
        std::cerr << "      Critical HangMan Game ERROR: " << e.what() << std::endl;
        std::cout << "      The HangMan Game could t be completed." << std::endl;
    }
}



//      Implementez clasa Room. Aceasta va fi responsabila de camerele din joc. In fiecare camera in care va intra
//   player ul, voi avea diferite iteme, entitati, dar si puzzle uri pe care acesta trebuie sa le rezolve. Ca
//   player ul sa poata trece dintr o camera in alta, acesta trebuie sa gaseasca CollectileItem ul specific usii
//   respective. Daca avanseaza dintr o camera in alta, va primi si un punctaj in plus la scor.

class Room {
private:
    std::string room_name;
    bool doorLocked;
    Container<Item> roomItems;    // folosesc clasa template pentru a gestiona obiectele
    Container<Entity> roomEntity;
    Container<SpecialEnemy> roomSpecialEnemies;

public:
    // constructor fara parametrii
    Room() {
        this->room_name = "Unnamed Room";
        this->doorLocked = false;
    }

    // constructor cu parametrii
    Room(const std::string& room_name, bool doorLocked) {
        this->room_name = room_name;
        this->doorLocked = doorLocked;
    }

    // constructorul de copiere
    Room(const Room& room) {
        this->room_name = room.room_name;
        this->doorLocked = room.doorLocked;
        this->roomItems = room.roomItems;
        this->roomEntity = room.roomEntity;
        this->roomSpecialEnemies = room.roomSpecialEnemies;
    }

    // operatorul de atribuire
    Room& operator=(const Room& room) {
        if (this == &room) {
            this->room_name = room.room_name;
            this->doorLocked = room.doorLocked;
            this->roomItems = room.roomItems;
            this->roomEntity = room.roomEntity;
            this->roomSpecialEnemies = room.roomSpecialEnemies;
        }
        return *this;
    }

    // supraincarcarea operatorului de scriere
    friend std::ostream& operator<<(std::ostream& out, const Room& room) {
        out << "    Room Name: " << room.room_name << std::endl;
        out << "    Door Locked: " << (room.doorLocked ? "Yes" : "No") << std::endl;

        if (!room.roomItems.ContainerIsEmpty()) {
            out << "    Number of Room Items: " << room.roomItems.GetSize() << std::endl;
            for (int i = 0; i < room.roomItems.GetSize(); i++) {
                out << *room.roomItems.GetElement(i);
            }
        }
        else {
            out << "    There are no items in the room." << std::endl;
        }
        out << std::endl;

        if (!room.roomEntity.ContainerIsEmpty()) {
            out << "    Number of Room Enemies: " << room.roomEntity.GetSize() << std::endl;
            for (int i = 0; i < room.roomEntity.GetSize(); i++) {
                out << *room.roomEntity.GetElement(i);
            }
        }
        else {
            out << "    There are no entity in the room." << std::endl;
        }
        out << std::endl;

        if (!room.roomSpecialEnemies.ContainerIsEmpty()) {
            out << "    Number of Room Special Enemies: " << room.roomSpecialEnemies.GetSize() << std::endl;
            for (int i = 0; i < room.roomSpecialEnemies.GetSize(); i++) {
                out << *room.roomSpecialEnemies.GetElement(i);
            }
        }
        else {
            out << "    There are no entity in the room." << std::endl;
        }
        out << std::endl;
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, Room& room) {
        std::cout << "      Enter room name: ";
        std::getline(in, room.room_name);

        int isUnlockedInt;
        std::cout << "      Is room unlocked? (1 = yes, 0 = no): ";
        in >> isUnlockedInt;
        room.doorLocked = (isUnlockedInt != 0);
        in.ignore();

        return in;
    }

    // metoda publica de afisare a datelor
    void DisplayRoomInfo() const {
        std::cout << *this;
    }

    // metoda publica de citire a datelor
    void ReadRoomInfo() {
        std::cin >> *this;
    }

    // getteri
    const std::string& GetRoomName() const {
        return this->room_name;
    }
    bool GetRoomDoorLocked() const {
        return this->doorLocked;
    }
    std::shared_ptr<Item> GetRoomItem(int index) const {
        return roomItems.GetElement(index);
    }
    int GetRoomItemCount() const {
        return static_cast<int>(roomItems.GetSize());
    }
    std::shared_ptr<Entity> GetRoomEntity(int index) const {
        return roomEntity.GetElement(index);
    }
    int GetRoomEntityCount() const {
        return static_cast<int>(roomEntity.GetSize());
    }
    std::shared_ptr<SpecialEnemy> GetRoomSpecialEnemies(int index) const {
        return roomSpecialEnemies.GetElement(index);
    }
    int GetRoomSpecialEnemiesCount() const {
        return static_cast<int>(roomSpecialEnemies.GetSize());
    }

    // setteri
    void SetRoomName(const std::string& room_name) {
        this->room_name = room_name;
    }
    void SetRoomDoorLocked(bool doorLocked) {
        this->doorLocked = doorLocked;
    }

    // functii de a adauga iteme, entitati si puzzle uri
    void AddRoomItem(const std::shared_ptr<Item>& item) {
        roomItems.AddElement(item);
    }
    void AddRoomEntity(const std::shared_ptr<Entity>& entity) {
        roomEntity.AddElement(entity);
    }
    void AddRoomSpecialEnemy(const std::shared_ptr<SpecialEnemy>& special_enemy) {
        roomSpecialEnemies.AddElement(special_enemy);
    }

    // functii pentru a sterge cate un obiect din fiecare inventar
    void RemoveItemFromRoom(int index) {
        roomItems.RemoveElement(index);
    }
    void RemoveEntityFromRoom(int index) {
        roomEntity.RemoveElement(index);
    }
    void RemoveSpecialEnemyFromRoom(int index) {
        roomSpecialEnemies.RemoveElement(index);
    }

    // functie pentru a sterge toate itemele din inventarul camerei
    void ClearRoomItems() {
        roomItems.ClearContainer();
    }
    void ClearRoomEntity() {
        roomEntity.ClearContainer();
    }
    void ClearRoomSpecialEnemy() {
        roomSpecialEnemies.ClearContainer();
    }

    // pt verificarea usii
    bool RoomDoorIsLocked() const {
        return this->doorLocked;
    }

    // metode pentru a incuia/descuia usa
    void UnlockRoomDoor() {
        this->doorLocked = false;
    }
    void LockRoomDoor() {
        this->doorLocked = true;
    }

    // destructor
    ~Room() {
        // eliberez memoria
        roomItems.ClearContainer();
        roomEntity.ClearContainer();
        roomSpecialEnemies.ClearContainer();
    }
};

// Definesc o clasa GameException in care gestionez majoritatea exceptiilor din joc
class GameException : public std::exception {
private:
    std::string message;
    int errorCode;

public:
    // definesc coduri de eroare pentru diferitele erori ale jocului
    enum ErrorCode {
        // erori generale
        UNKNOWN_ERROR = 0,
        INVALID_INPUT = 1,

        // erori legate de camere
        ROOM_NOT_FOUND = 100,
        ROOM_CONNECTION_NOT_FOUND = 101,
        ROOM_LOCKED = 102,

        // erori legate de personaj
        CHARACTER_NOT_FOUND = 200,
        CHARACTER_NAME_INVALID = 201,
        CHARACTER_NO_HEALTH = 202,

        // erori legate de iteme
        ITEM_NOT_FOUND = 300,
        ITEM_TYPE_INVALID = 301,

        // erori legate de battle
        COMBAT_ERROR = 400,
        NO_WEAPON_EQUIPPED = 401
    };

    // constructor implicit
    GameException(int code, const std::string& msg)
        : errorCode(code), message(msg) {}

    // constructor cu mesaj
    explicit GameException(int code) : errorCode(code) {
        switch (code) {
            // erori generale
            case UNKNOWN_ERROR:
                message = "Unknown error in the game.";
                break;
            case INVALID_INPUT:
                message = "Invalid input. Please try again.";
                break;

            // erori camere
            case ROOM_NOT_FOUND:
                message = "The specified room was not found.";
                break;
            case ROOM_CONNECTION_NOT_FOUND:
                message = "There is no connection between these rooms.";
                break;
            case ROOM_LOCKED:
                message = "The room is locked. You need a key to open it.";
                break;

            // erori player
            case CHARACTER_NOT_FOUND:
                message = "The specified character was not found.";
                break;
            case CHARACTER_NAME_INVALID:
                message = "The character name is invalid.";
                break;
            case CHARACTER_NO_HEALTH:
                message = "The character has no life left.";
                break;

            // erori item
            case ITEM_NOT_FOUND:
                message = "The specified object was not found.";
                break;
            case ITEM_TYPE_INVALID:
                message = "The object type is not valid for this action.";
                break;

            // Erori legate de luptă
            case COMBAT_ERROR:
                message = "Error in the combat system.";
                break;
            case NO_WEAPON_EQUIPPED:
                message = "You have no weapons equipped for combat.";
                break;

            default:
                message = "Eroare nedefinită.";
        }
    }

    // suprascriu metoda what() din std::exception
    const char* what() const noexcept override {
        return message.c_str();
    }

    // getteri
    int GetErrorCode() const {
        return errorCode;
    }

    // metode helper
    static GameException RoomNotFound(const std::string& roomName) {
        return GameException(ROOM_NOT_FOUND, "Room \"" + roomName + "\" not found!");
    }

    static GameException RoomConnectionNotFound(const std::string& fromRoom, const std::string& toRoom) {
        return GameException(ROOM_CONNECTION_NOT_FOUND,
                              "There is no connection between the rooms \"" + fromRoom + "\" and \"" + toRoom + "\"!");
    }

    static GameException CharacterNotFound(const std::string& characterName) {
        return GameException(CHARACTER_NOT_FOUND, "The player \"" + characterName + "\" was not found!");
    }

    static GameException ItemNotFound(const std::string& itemName) {
        return GameException(ITEM_NOT_FOUND, "Item \"" + itemName + "\" was not found!");
    }

    static GameException NoWeaponEquipped(const std::string& characterName) {
        return GameException(NO_WEAPON_EQUIPPED,
                              "Player \"" + characterName + "\" has no weapon equipped for combat!");
    }

    static GameException RoomLocked(const std::string& roomName, const std::string& keyName) {
        return GameException(ROOM_LOCKED,
                              "Room \"" + roomName + "\" is locked. You need \"" + keyName + "\" to open it!");
    }
};

class MenuException : public GameException {
    public:
    explicit MenuException(const std::string& msg) : GameException(*"[MENU ERROR]: ", msg) {}
};

/*      Definesc clasa Game. Aceasta detine controlul asupra jocului.
 *          Principale functionalitati:
 *              - gestioneaza camerele din joc
 *              - controleaza personajul principal si parcursul acestuia
 *              - se implementeaza logica jocului (colectarea itemelor, lupta cu entitatile malefice, interactiunea
 *              cu entitatile pozitive, rezolvarea puzzlelor (ghicitori si spanzuratoarea), avansarea prin camere)
 *              - gestioneaza meniul interactiv al jocului
 *
 *      Aspecte despre meniul interactiv:
 *          - sa am un ExitGame care sa functioneze pentru tot parcursul jocului.
 *          - initialele functionalitati (1. Set player name, 2. Start Game, 3. View Character, 4. View the best
 *          3 scores of the game completion, 5. Exit Game.
 *
 *          Pentru StartGame:
 *              - la inceputul jocului, playerului i se afiseaza o harta a spitalului cu toate camerele din joc
 *              (important, nu poti trece din orice camera in orice camera - exista oarecum un traseu pe care poti
 *              merge), pe parcursul avansarii dintr o camera in alta, playerului i se afiseaza pe ecran pe harta
 *              exact camera in care se afla.
 *              - din camere pot colecta diferite iteme (de atac, de heal sau colectibile), pot gasi diferite
 *              puzzle-uri pe care sa le rezolv ca sa primesc un nou item si sa primesc un scor in plus, ma pot
 *              lupta cu entitatile malefice (Enemy sau SpecialEnemy) sau pot intercationa cu NPC-urile pentru
 *              a primi hint uri despre joc.
 *              - am mai multe camere, cu legaturi intre ele, iar playerul poate avansa dintr-o camera in alta
 *              doar daca are in inventar CollectibleItem-ul necesar pentru a descuia respectiva usa.
 *              - pentru a evada din spital (a termina si castiga jocul) trebuie sa ajung in camera speciala cu
 *              usa pentru a evada si trebuie sa am anumite iteme pentru a deschide usa.
 *              - cand ma aflu intr-o anumita camera, am doar anumite variante de a avansa in alte camere ->
 *              pot alege doar o singura camera.
 *              - pentru a avansa in alta camera, trebuie neaparat sa o descui, dar pe parcursul jocului ma pot
 *              intoarce in camerele deja descuiate.
 */

class Game {
private:
    Character *player;
    std::vector<Room> rooms;
    std::map<std::string, std::vector<std::string>> roomConnections;   // conectiunile dintre camere
    std::string currentRoomName;

public:
    // definesc constructorul fara parametrii
    Game() : currentRoomName("") {
        SetupRooms();
        SetupConnections();
    }

    // definesc constructorul cu parametrii
    Game(Character& player, const std::vector<Room>& rooms, const std::string& startRoom)
        : player(&player), rooms(rooms), currentRoomName(startRoom) {
        SetupConnections();    // conexiunile sunt generate pe baza camerelor
    }

    // constructorul de copiere
    Game(const Game& game)
        : player(game.player), rooms(game.rooms), roomConnections(game.roomConnections),
        currentRoomName(game.currentRoomName) {}

    // operatorul de atribuire
    Game& operator=(const Game& game) {
        if (this != &game) {
            this->player = game.player;
            this->rooms = game.rooms;
            this->roomConnections = game.roomConnections;
            this->currentRoomName = game.currentRoomName;
        }
        return *this;
    }

    // supraincarcarea operatorului de afisare / scriere
    friend std::ostream& operator<<(std::ostream& out, const Game& game) {
        out << "    Player: " << game.player << std::endl;
        out << "    Rooms: " << std::endl;
        for (const auto& room : game.rooms) {
            out << "        " << room << std::endl;
        }
        out << "    Room Connections: " << std::endl;
        for (const auto& connection : game.roomConnections) {
            out << "    " << connection.first << std::endl;
        }
        out << "    Current Room: " << game.currentRoomName << std::endl;
        return out;
    }

    // supraincarcarea operatorului de citire
    friend std::istream& operator>>(std::istream& in, const Game& game) {
        std::cout << "    Enter player name: ";
        std::string playerName;
        std::getline(in, playerName);
        game.player->SetCharacterName(playerName);
        return in;
    }

    // destructorul clasei Game
    ~Game() {
        delete player;
        rooms.clear();
        roomConnections.clear();
    }

    // metode de afisare si citire
    void DisplayGameInfo() const {
        std::cout << *this << std::endl;
    }

    void ReadGameInfo() const {
        std::cin >> *this;
    }

    //getteri
    Character& GetPlayer() const {
        return *player;
    }

    std::vector<Room> GetRooms() const {
        return this->rooms;
    }

    std::string GetCurrentRoomName() const {
        return this->currentRoomName;
    }

    // setteri
    void SetPlayer(Character& player) {
        this->player = &player;
    }
    void SetRooms(const std::vector<Room>& rooms) {
        this->rooms = rooms;
    }
    void SetRoomConnections(const std::map<std::string, std::vector<std::string>>& roomConnections) {
        this->roomConnections = roomConnections;
    }
    void SetCurrentRoomName(const std::string& currentRoomName) {
        this->currentRoomName = currentRoomName;
    }

    // setez meniul jocului
    void ShowMenu() {
        try {
            int option;
            do {
                std::cout << "             =====================================" << std::endl;
                std::cout << "                           Main menu              " << std::endl;
                std::cout << "             =====================================" << std::endl;
                std::cout << std::endl;
                std::cout << "             1. Play game" << std::endl;
                std::cout << "             2. Exit" << std::endl;
                option = ReadValidatedInput<int>("          Enter your choice: ");

                switch (option) {
                    case 1: {
                        std::cout << std::endl;
                        std::cout << "                * * * * * * * * * * * * * * *" << std::endl;
                        std::cout << "                *    Welcome to the game!   *" << std::endl;
                        std::cout << "                * * * * * * * * * * * * * * *" << std::endl;
                        std::cout << std::endl;
                        std::cout << "     You wake up in a dark room in an abandoned hospital. The only way \n"
                                "  to escape is to unlock the main door. You have to hurry, because evil \n"
                                "  entities are after you. Collect all the necessary items, solve all the \n"
                                "  puzzles and destroy the enemies, time is running out very quickly!\n";
                        std::cout << std::endl;
                        std::cout << "   Do you want to play? (yes/no): ";

                        try {
                            std::string choice;
                            // verific daca input ul este ok
                            if (!(std::cin >> choice)) {
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                throw InputException("Invalid input. Please type 'yes' or 'no'.");
                            }

                            // convertesc la lowercase
                            std::transform(choice.begin(), choice.end(), choice.begin(),
                                [](unsigned char c) {return std::tolower(c);});

                            if (choice != "yes" && choice != "no") {
                                throw InputException("Please type 'yes' or 'no'.");
                            }

                            if (choice == "yes") {
                                int choice1;
                                do {
                                    try {
                                        std::cout << "   1. Set player name." << std::endl;
                                        std::cout << "   2. Start game!" << std::endl;
                                        std::cout << "   3. View Character" << std::endl;
                                        std::cout << "   4. Exit Game:(" << std::endl;
                                        std::cout << "   Enter your choice: ";

                                        if (!(std::cin >> choice1)) {
                                            std::cin.clear();
                                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                            throw InputException("Please enter a valid number.");
                                        }
                                        std::cin.ignore();

                                        switch (choice1) {
                                            case 1:
                                                try {
                                                    SetPlayerName();
                                                } catch (const GameException& e) {
                                                    std::cerr << "      Error setting player name: " << e.what() << std::endl;
                                                }
                                                break;
                                            case 2:
                                                try {
                                                    StartGame();
                                                } catch (const GameException& e) {
                                                    std::cerr << "   Game error: " << e.what() << std::endl;
                                                    std::cout << "   Returning to menu..." << std::endl;
                                                } catch (const std::exception& e) {
                                                    std::cerr << "   Unexpected error during game: " << e.what() << std::endl;
                                                    std::cout << "   Returning to menu..." << std::endl;
                                                }
                                            break;
                                            case 3:
                                                try {
                                                    ViewCharacter();
                                                } catch (const GameException& e) {
                                                    std::cerr << "   Error viewing character: " << e.what() << std::endl;
                                                }
                                            break;
                                            case 4: {
                                                std::cout << "   Exiting the game..." << std::endl;
                                                try {
                                                    ExitGame();
                                                } catch (const std::exception& e) {
                                                    std::cerr << "   Error during exit: " << e.what() << std::endl;
                                                    std::cout << "   Forcing exit..." << std::endl;
                                                    exit(1); // fortez iesirea din joc in caz de eroare
                                                }
                                                break;
                                            }
                                            default:
                                                std::cout << "   Invalid option! Try again." << std::endl;
                                        }
                                    } catch (const InputException& e) {
                                        std::cerr << "      [INPUT ERROR]: " << e.what() << std::endl;
                                    } catch (const MenuException& e) {
                                        std::cerr << "      [MENU ERROR]: " << e.what() << std::endl;
                                    }
                                    std::cout << std::endl;
                                } while (choice1 != 4);
                            }
                            else {
                                try {
                                    ExitGame();
                                } catch (const std::exception& e) {
                                    std::cerr << "   Error during exit: " << e.what() << std::endl;
                                    std::cout << "   Forcing exit..." << std::endl;
                                    exit(1);
                                }
                            }
                        } catch (const InputException& e) {
                            std::cerr << "      [ERROR]: " << e.what() << std::endl;
                            std::cout << "      Returning the main menu..." << std::endl;
                        }
                        break;
                    }
                    case 2: {
                        try {
                            ExitGame();
                        } catch (const std::exception& e) {
                            std::cerr << "      [ERROR during the exit]: " << e.what() << std::endl;
                            std::cout << "      Forcing exit..." << std::endl;
                            exit(1);
                        }
                        break;
                    }
                    default:
                        throw MenuException("Invalid option! Please choose 1 or 2.");
                }
                std::cout << std::endl;
            } while (option != 2);
        } catch (const std::exception& e) {
            std::cerr << "      Critical error in menu: " << e.what() << std::endl;
            std::cout << "      The game will now exit." << std::endl;
            exit(1);
        }
    }

    // functie pentru setarea numelui jucatorului
    void SetPlayerName() {
        try {
            std::string name;
            std::cout << "   Enter your character's name: ";
            std::getline(std::cin, name);

            if (name.empty()) {
                throw InputException("Name cannot be empty.");
            }

            if (name.length() > 20) {
                throw InputException("Name is too long. Maximum 20 characters allowed.");
            }

            // Verificăm dacă numele conține caractere invalide
            for (char c : name) {
                if (!std::isalnum(c) && c != ' ' && c != '_' && c != '-') {
                    throw InputException("Name contains invalid characters. Use only letters, numbers, spaces, underscores, and hyphens.");
                }
            }

            if (player != nullptr) {
                delete player;  // sterg playerul anterior daca exista
            }

            player = new Character(name, 100, 0);
            std::cout << "   Character name set to: " << name << std::endl;
            std::cout << std::endl;
            std::cout << "   Hello, " << player->GetCharacterName() << "! Are you ready for adventure?" << std::endl;
        } catch (const InputException& e) {
            std::cerr << "   Error setting name: " << e.what() << std::endl;
            throw; // propag exceptia pt a fi prinsa in functia apelanta
        } catch (const std::exception& e) {
            std::cerr << "   Unexpected error: " << e.what() << std::endl;
            throw;
        }
    }

    // functie pentru afisarea caracterului
    void ViewCharacter() {
        if (player == nullptr) {
            std::cout << "  Player name is not set! Please set it first." << std::endl;
            return;
        }
        std::cout << "  Details about your character:" << std::endl;
        player->DisplayInfoCharacter();
    }

    // functie pentru exit ul din game
    static void ExitGame() {
        std::cout << std::endl;
        std::cout << "      Exit the game. Goodbye!" << std::endl;
        exit(0);
    }

    // functie pentru afisarea hartii spitalului
    static void DrawHospitalMap(const std::string& room_name) {
        if (room_name == "Reception") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATIONG ROOM   /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /  -CURRENT-     /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |    -ROOM-      |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Newborn Nursery") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |  -CURRENT-  |                     |                   |                             |\n"
                "           |   -ROOM-    |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Operating Room") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |    -CURRENT ROOM-   |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Room For Elderly") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |   -CURRENT ROOM-  |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Autopsy Room") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |       -CURRENT ROOM-        |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Medication Room") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |   -CURRENT-    |             |                                     |\n"
                "           |                |    -ROOM-      |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |   LOBBY     | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Lobby") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |  -CURRENT-  |                   |                 |\n"
                "           |              |                  |   -ROOM-    |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Room With Emergency Exit") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |          -CURRENT ROOM-             |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Document Archive") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |  -CURRENT-   |                  |             |                   |                 |\n"
                "           |   -ROOM-     |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Sanatorium") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /  -CURRENT-       /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |    -ROOM-        |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Room With Xray Equipment") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |   -CURRENT ROOM-  |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
        else if (room_name == "Room For Materials") {
            std::cout << "      This is the map of the abandoned hospital:" << std::endl;
            std::cout << std::endl;
            std::cout <<
                "           + - - - - - - + - - - - - - - - - - + - - - - - - - - - + - - - - - - - - - - - - - - +\n"
                "           |             |                     |                   |                             |\n"
                "           |   NEWBORN   |                     |    ROOM FOR THE   |          AUTOPSY            |\n"
                "           |   NURSERY    /   OPERATING ROOM    /     ELDERLY       /          ROOM              |\n"
                "           |             |                     |                   |                             |\n"
                "           |             |                     |                   |                             |\n"
                "           + - - /   - - + - - - - - - - - - - + - - - - - - - - - + - - - /   - - - - - - - - - +\n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "  START      /  RECEPTION   |   MEDICATION   |             |        ROOM WITH EMERGENCY            /   END GAME   \n"
                "  ---->     /                /    ROOM        /             /             EXIT                    /     ---->      \n"
                "           |                |                |             |                                     |\n"
                "           |                |                |             |                                     |\n"
                "           + - - - /   - - - - - - - - - - - |    LOBBY    | - - - - - - - - - + - - - /   - - - +\n"
                "           |              |                  |             |                   |                 |\n"
                "           |              |                  |             |                   |                 |\n"
                "           |   DOCUMENT   |   SANATORIUM     |             |   ROOM WITH X-RAY |   ROOM FOR      |\n"
                "           |   ARCHIVE     /                  /             /    EQUIPMENT      /  MATERIALS     |\n"
                "           |              |                  |             |                   | -CUURENT ROOM-  |\n"
                "           |              |                  |             |                   |                 |\n"
                "           + - - - - - - - - + - - - - - - - - - - - - - - + - - - - - - - - - + - - - - - - - - +\n"
            << std::endl;
        }
    }

    // functia pentru a seta camerele si a adauga itemele in si entitatile
    void SetupRooms() {
        // creez itemele pentru a le adauga in fiecare camera

        // iteme de atac
        std::shared_ptr<AttackItem> sword = std::make_shared<AttackItem>("Sword", "Dangerous", 100);
        std::shared_ptr<AttackItem> fireball_spell = std::make_shared<AttackItem>("Fireball", "Dangerous", 100);
        std::shared_ptr<AttackItem> poison_dagger = std::make_shared<AttackItem>("Poison Dagger", "Dangerous", 40);
        std::shared_ptr<AttackItem> dual_dagger = std::make_shared<AttackItem>("Dual Dagger", "Dangerous", 60);
        std::shared_ptr<AttackItem> bloodthorn_dagger = std::make_shared<AttackItem>("Bloodthorn Dagger", "Dangerous", 35);
        std::shared_ptr<AttackItem> stormbringer_gauntlet = std::make_shared<AttackItem>("Stormbring Gauntlet", "Dangerous", 45);
        std::shared_ptr<AttackItem> venomfang_blade = std::make_shared<AttackItem>("Venomfang Blade", "Black Magic", 20);
        std::shared_ptr<AttackItem> soulfire_orb = std::make_shared<AttackItem>("Soulfire Orb", "Black Magic", 10);
        std::shared_ptr<AttackItem> doomhammer = std::make_shared<AttackItem>("Domhammer", "Dangerous", 15);
        std::shared_ptr<AttackItem> frostbite_arrow = std::make_shared<AttackItem>("Frost Arrow", "Dangerous", 20);
        std::shared_ptr<AttackItem> infernal_grenade = std::make_shared<AttackItem>("Infernal Grenade", "Dangerous", 25);
        std::shared_ptr<AttackItem> shadowfang_scythe = std::make_shared<AttackItem>("ShadowFang Scythe", "Dangerous", 30);
        std::shared_ptr<AttackItem> voidcaller_staff = std::make_shared<AttackItem>("Voidcaller Staff", "Black Magic", 40);
        std::shared_ptr<AttackItem> thunderstrike_javelin = std::make_shared<AttackItem>("Thunderstrike Javelin", "Black Magic", 35);
        std::shared_ptr<AttackItem> phoenix_feather_spear = std::make_shared<AttackItem>("Phoenix Feather", "Black Magic", 45);
        std::shared_ptr<AttackItem> necrotic_chains = std::make_shared<AttackItem>("Necrotic Chains", "Black Magic", 20);
        std::shared_ptr<AttackItem> crystalized_plasma_rifle = std::make_shared<AttackItem>("Crystal Plasma Rifle", "Dangerous", 30);
        std::shared_ptr<AttackItem> sunflare_chakram = std::make_shared<AttackItem>("Sunflare Chakram", "Dangerous", 25);
        std::shared_ptr<AttackItem> deathspike_bolts = std::make_shared<AttackItem>("Deathspike Bolts", "Dangerous", 10);
        std::shared_ptr<AttackItem> warpblade_cutlass = std::make_shared<AttackItem>("Warpblade Cutlass", "Dangerous", 40);
        std::shared_ptr<AttackItem> hellhound_claws = std::make_shared<AttackItem>("Hellhound Claws", "Black Magic", 30);
        std::shared_ptr<AttackItem> eldritch_bone_wand = std::make_shared<AttackItem>("Eldritch Bone Wand", "Black Magic", 55);
        std::shared_ptr<AttackItem> meteorite_axe = std::make_shared<AttackItem>("Meteorite Axe", "Dangerous", 30);
        std::shared_ptr<AttackItem> toxic_cloud_vial = std::make_shared<AttackItem>("Toxic Cloud Vial", "Black Magic", 40);
        std::shared_ptr<AttackItem> dagger_of_echoes = std::make_shared<AttackItem>("Dagger Of Echoes", "Dangerous", 50);
        std::shared_ptr<AttackItem> celestial_warhammer = std::make_shared<AttackItem>("Celestial Warhammer", "Dangerous", 35);
        std::shared_ptr<AttackItem> bloodletter_katar = std::make_shared<AttackItem>("Bloodletter Katar", "Black Magic", 30);
        std::shared_ptr<AttackItem> molten_chainsaw = std::make_shared<AttackItem>("Molten Chainsaw", "Dangerous", 45);
        std::shared_ptr<AttackItem> gravity_bomb = std::make_shared<AttackItem>("Gravity Bomb", "Dangerous", 55);
        std::shared_ptr<AttackItem> soulstealer_fang = std::make_shared<AttackItem>("Soulster Fang", "Black Magic", 45);
        std::shared_ptr<AttackItem> cursed_marionette_strings = std::make_shared<AttackItem>("Cursed Marionette", "Black Magic", 35);
        std::shared_ptr<AttackItem> glacial_trident = std::make_shared<AttackItem>("Glacial Trident ", "Dangerous", 25);
        std::shared_ptr<AttackItem> ebonflame_crossbow = std::make_shared<AttackItem>("Ebonflame Crossbow", "Dangerous", 40);

        // iteme de viata
        std::shared_ptr<HealItem> elixir_of_eternal_vitality = std::make_shared<HealItem>("Elixir of eternal Vitality", "Elixir", 35);
        std::shared_ptr<HealItem> moonflower_essence = std::make_shared<HealItem>("Moonflower Essence", "Elixir", 45);
        std::shared_ptr<HealItem> sunleaf_brew = std::make_shared<HealItem>("Sunleaf Brew", "Elixir", 20);
        std::shared_ptr<HealItem> phoenix_down = std::make_shared<HealItem>("Phoenix Down", "Mystical", 40);
        std::shared_ptr<HealItem> angelic_tear = std::make_shared<HealItem>("Angelic Tear", "Elixir", 30);
        std::shared_ptr<HealItem> mystic_honeycomb = std::make_shared<HealItem>("Mystic Honeycomb", "Mystical", 25);
        std::shared_ptr<HealItem> druid_healing_balm = std::make_shared<HealItem>("Druid Healing Balm", "Elixir", 50);
        std::shared_ptr<HealItem> celestial_dewdrop = std::make_shared<HealItem>("Celestial Dewardrop", "Elixir", 25);
        std::shared_ptr<HealItem> lifestone_amulet = std::make_shared<HealItem>("Lifestone Amulet", "Mystical", 35);
        std::shared_ptr<HealItem> spirit_infused_tea = std::make_shared<HealItem>("Spirit Infused Tea", "Elixir", 30);
        std::shared_ptr<HealItem> crimson_berry_tonic = std::make_shared<HealItem>("Crimson Berry Tonic", "Elixir", 45);
        std::shared_ptr<HealItem> sacred_lotus_petal = std::make_shared<HealItem>("Sacred Lotus Petal", "Mystical", 25);
        std::shared_ptr<HealItem> etherial_mending_orb = std::make_shared<HealItem>("Etherial Mending", "Mystical", 30);
        std::shared_ptr<HealItem> glowing_herbal_poultice = std::make_shared<HealItem>("Glowing Herbal Poultice", "Elixir", 35);
        std::shared_ptr<HealItem> warmth_of_the_ancestors = std::make_shared<HealItem>("Warmth of The Ancestors", "Mystical", 25);
        std::shared_ptr<HealItem> holy_water_flask = std::make_shared<HealItem>("Holy water flask", "Elixir", 45);
        std::shared_ptr<HealItem> golden_nectar_of_the_fae = std::make_shared<HealItem>("Golden Nectar of The Fae", "Elixir", 25);
        std::shared_ptr<HealItem> radiant_sapphire_medallion = std::make_shared<HealItem>("Radiant Sapphire Medallion", "Mystical", 20);
        std::shared_ptr<HealItem> giant_heart_decoction = std::make_shared<HealItem>("Giant Heartde Decoding", "Elixir", 25);
        std::shared_ptr<HealItem> rejuvenation_rune = std::make_shared<HealItem>("Rejuvenation Rune", "Mystical", 30);

        // iteme colectibile
        std::shared_ptr<CollectibleItem> runestone_of_the_forgotten = std::make_shared<CollectibleItem>("Runestone of the Forgotten", "Mystical", "Rare");
        std::shared_ptr<CollectibleItem> astral_key = std::make_shared<CollectibleItem>("Astral Key", "Magic", "Unique");
        std::shared_ptr<CollectibleItem> phoenix_crest_medallion = std::make_shared<CollectibleItem>("Phoenix Crest Medallion", "Mystical", "Rare");
        std::shared_ptr<CollectibleItem> frozen_core_shard = std::make_shared<CollectibleItem>("Frozen Core Shard", "Magic", "Rare");
        std::shared_ptr<CollectibleItem> clockwork_gear_of_pessage = std::make_shared<CollectibleItem>("Clockwork Gear of Pessage", "Mystical", "Unique");
        std::shared_ptr<CollectibleItem> eldritch_obsidian_fragment = std::make_shared<CollectibleItem>("Eldritch Obsidian Fragment", "Magic", "Rare");
        std::shared_ptr<CollectibleItem> sunfire_seal = std::make_shared<CollectibleItem>("Sunfire Seal", "Magic", "Rare");
        std::shared_ptr<CollectibleItem> chalice_of_the_moon = std::make_shared<CollectibleItem>("Chalice of Moon", "Mystical", "Unique");
        std::shared_ptr<CollectibleItem> dragonfang_talisman = std::make_shared<CollectibleItem>("Dragon Talisman", "Mystical", "Unique");
        std::shared_ptr<CollectibleItem> eye_of_the_guardian = std::make_shared<CollectibleItem>("Eye of Guardian", "Magic", "Unique");
        std::shared_ptr<CollectibleItem> map = std::make_shared<CollectibleItem>("Map", "Magic", "Unique");
        std::shared_ptr<CollectibleItem> master_key = std::make_shared<CollectibleItem>("Master Key", "Significant", "Unique");

        // creez entitatile pentru a le adauga in fiecare camera
        std::shared_ptr<Enemy> ghost = std::make_shared<Enemy>("Ghost", 80, 30, "periculos", 3);
        std::shared_ptr<Enemy> demon = std::make_shared<Enemy>("Demon", 85, 25, "periculos", 2);
        std::shared_ptr<Enemy> slenderman = std::make_shared<Enemy>("Slenderman", 100, 40, "periculos", 5);
        std::shared_ptr<Enemy> the_warden = std::make_shared<Enemy>("The Warden", 90, 25, "perisulos", 3);
        std::shared_ptr<Enemy> nurse_hollow = std::make_shared<Enemy>("Nurse Hollow", 80, 25, "periculos", 2);
        std::shared_ptr<Enemy> the_screamer = std::make_shared<Enemy>("The Screamer", 90, 25, "periculos", 3);
        std::shared_ptr<Enemy> the_crawling_one = std::make_shared<Enemy>("The Crawling One", 80, 25, "periculos", 2);
        std::shared_ptr<Enemy> the_shroud = std::make_shared<Enemy>("The Shroud", 80, 25, "periculos", 3);
        std::shared_ptr<Enemy> echo_patiend = std::make_shared<Enemy>("Echo Patiend", 80, 25, "periculos", 2);
        std::shared_ptr<Enemy> the_hung_choir = std::make_shared<Enemy>("The Hung Choir", 80, 25, "periculos", 3);
        std::shared_ptr<Enemy> the_plague_host = std::make_shared<Enemy>("The Plague Host", 80, 25, "periculos", 2);
        std::shared_ptr<Enemy> twitching_marionette = std::make_shared<Enemy>("Twitching Marionette", 80, 25, "periculos", 3);
        std::shared_ptr<Enemy> the_hollow_infant = std::make_shared<Enemy>("The Hollow Infant", 80, 25, "periculos", 2);
        std::shared_ptr<Enemy> the_sealed_surgeon = std::make_shared<Enemy>("The Sealed Surgeon", 80, 25, "periculos", 3);

        // creez special enemies pentru a le adauga in fiecare camera
        std::shared_ptr<SpecialEnemy> flame_revenant = std::make_shared<SpecialEnemy>("Flame Revenant", 80, 25, "periculos", 3, "Firebrand Sword", "Fire", 30);
        std::shared_ptr<SpecialEnemy> venom_priestess = std::make_shared<SpecialEnemy>("Venom Priestess", 100, 20, "Assassin", 4, "Toxic Dagger", "Poison", 25);
        std::shared_ptr<SpecialEnemy> frost_knight = std::make_shared<SpecialEnemy>("Frost Knight", 100, 20, "Undead", 6, "Icebreaker Axe", "Ice", 35);
        std::shared_ptr<SpecialEnemy> doom_herald = std::make_shared<SpecialEnemy>("Doom Herald", 100, 25, "Dark Mage", 7, "Shadow Whip", "Shadow", 40);
        std::shared_ptr<SpecialEnemy> strom_harpy = std::make_shared<SpecialEnemy>("Storm Harpy", 100, 20, "Beast", 4, "Thunder Claws", "Electric", 28);
        std::shared_ptr<SpecialEnemy> plague_monk = std::make_shared<SpecialEnemy>("Plague Monk", 100, 20, "Cursed Human", 3, "Infected Staff", "Disease", 25);
        std::shared_ptr<SpecialEnemy> arhen_warlord = std::make_shared<SpecialEnemy>("Ashen Warlord", 100, 30, "Warrior", 8, "Volcanic Hammer", "Fire", 45);
        std::shared_ptr<SpecialEnemy> ghostblade_assassin = std::make_shared<SpecialEnemy>("Ghostblade Assassin", 90, 25, "Spirit", 5, "Phantom Blade", "Magic", 35);
        std::shared_ptr<SpecialEnemy> iron_revenat = std::make_shared<SpecialEnemy>("Iron Revenant", 100, 25, "Construct", 9, "Crush Gauntlet", "Physical", 35);
        std::shared_ptr<SpecialEnemy> dark_siren = std::make_shared<SpecialEnemy>("Dark Siren", 100, 20, "Mythical", 5, "Echoing Spear", "Sound", 25);
        std::shared_ptr<SpecialEnemy> blood_mage = std::make_shared<SpecialEnemy>("Blood Mage", 100, 20, "Necromancer", 6, "Ritual Knife", "Dark", 20);
        std::shared_ptr<SpecialEnemy> lich_champion = std::make_shared<SpecialEnemy>("Lich Champion", 100, 30, "Undead", 8, "Soul Reaper", "Soul", 35);

        // creez camerele din desen si le adaug in logica jocului
        Room reception("Reception", false);
        reception.AddRoomItem(fireball_spell);
        reception.AddRoomItem(poison_dagger);
        reception.AddRoomItem(elixir_of_eternal_vitality);
        reception.AddRoomItem(moonflower_essence);
        reception.AddRoomItem(runestone_of_the_forgotten);
        reception.AddRoomEntity(demon);
        reception.AddRoomSpecialEnemy(flame_revenant);
        rooms.push_back(reception);

        Room newborn_nursery("Newborn Nursery", false);
        newborn_nursery.AddRoomItem(bloodthorn_dagger);
        newborn_nursery.AddRoomItem(stormbringer_gauntlet);
        newborn_nursery.AddRoomItem(sunleaf_brew);
        newborn_nursery.AddRoomItem(phoenix_down);
        newborn_nursery.AddRoomEntity(ghost);
        newborn_nursery.AddRoomEntity(the_warden);
        newborn_nursery.AddRoomItem(astral_key);
        newborn_nursery.AddRoomSpecialEnemy(venom_priestess);
        rooms.push_back(newborn_nursery);

        Room document_archive("Document Archive", false);
        document_archive.AddRoomItem(venomfang_blade);
        document_archive.AddRoomItem(soulfire_orb);
        document_archive.AddRoomItem(angelic_tear);
        document_archive.AddRoomItem(phoenix_crest_medallion);
        document_archive.AddRoomEntity(nurse_hollow);
        document_archive.AddRoomSpecialEnemy(frost_knight);
        rooms.push_back(document_archive);

        Room operating_room("Operating Room", false);
        operating_room.AddRoomItem(doomhammer);
        operating_room.AddRoomItem(frostbite_arrow);
        operating_room.AddRoomItem(mystic_honeycomb);
        operating_room.AddRoomItem(druid_healing_balm);
        operating_room.AddRoomItem(frozen_core_shard);
        operating_room.AddRoomEntity(the_screamer);
        operating_room.AddRoomSpecialEnemy(doom_herald);
        rooms.push_back(operating_room);

        Room medication_room("Medication Room", false);
        medication_room.AddRoomItem(infernal_grenade);
        medication_room.AddRoomItem(shadowfang_scythe);
        medication_room.AddRoomItem(druid_healing_balm);
        medication_room.AddRoomItem(clockwork_gear_of_pessage);
        medication_room.AddRoomEntity(the_shroud);
        medication_room.AddRoomSpecialEnemy(strom_harpy);
        rooms.push_back(medication_room);

        Room sanatorium("Sanatorium", false);
        sanatorium.AddRoomItem(voidcaller_staff);
        sanatorium.AddRoomItem(celestial_dewdrop);
        sanatorium.AddRoomItem(eldritch_bone_wand);
        sanatorium.AddRoomEntity(echo_patiend);
        sanatorium.AddRoomSpecialEnemy(plague_monk);
        rooms.push_back(sanatorium);

        Room room_for_elderly("Room For Elderly", false);
        room_for_elderly.AddRoomItem(thunderstrike_javelin);
        room_for_elderly.AddRoomItem(lifestone_amulet);
        room_for_elderly.AddRoomItem(sunfire_seal);
        room_for_elderly.AddRoomEntity(the_hung_choir);
        room_for_elderly.AddRoomSpecialEnemy(arhen_warlord);
        rooms.push_back(room_for_elderly);

        Room lobby("Lobby", false);
        lobby.AddRoomItem(phoenix_feather_spear);
        lobby.AddRoomItem(spirit_infused_tea);
        lobby.AddRoomItem(chalice_of_the_moon);
        lobby.AddRoomEntity(twitching_marionette);
        lobby.AddRoomSpecialEnemy(ghostblade_assassin);
        rooms.push_back(lobby);

        Room autopsy_room("Autopsy Room", false);
        autopsy_room.AddRoomItem(necrotic_chains);
        autopsy_room.AddRoomItem(crimson_berry_tonic);
        autopsy_room.AddRoomItem(dragonfang_talisman);
        autopsy_room.AddRoomEntity(the_hollow_infant);
        autopsy_room.AddRoomSpecialEnemy(iron_revenat);
        rooms.push_back(autopsy_room);

        Room room_emergency_exit("Room With Emergency Exit", true);
        room_emergency_exit.AddRoomItem(master_key);
        rooms.push_back(room_emergency_exit);

        Room room_xray_equipment("Room With Xray Equipment", false);
        room_xray_equipment.AddRoomItem(sunflare_chakram);
        room_xray_equipment.AddRoomItem(etherial_mending_orb);
        room_xray_equipment.AddRoomItem(map);
        room_xray_equipment.AddRoomEntity(slenderman);
        room_xray_equipment.AddRoomSpecialEnemy(blood_mage);
        rooms.push_back(room_xray_equipment);

        Room room_materials("Room For Materials", false);
        room_materials.AddRoomItem(crystalized_plasma_rifle);
        room_materials.AddRoomItem(sacred_lotus_petal);
        room_materials.AddRoomItem(eye_of_the_guardian);
        room_materials.AddRoomEntity(the_sealed_surgeon);
        room_materials.AddRoomSpecialEnemy(dark_siren);
        rooms.push_back(room_materials);

    }

    // functia care creeaza legaturile intre camerele din joc
    void SetupConnections() {
        roomConnections["Reception"] = {"Newborn Nursery", "Document Archive", "Medication Room"};
        roomConnections["Newborn Nursery"] = {"Operating Room"};
        roomConnections["Document Archive"] = {"Sanatorium"};
        roomConnections["Medication Room"] = {"Lobby"};
        roomConnections["Sanatorium"] = {"Lobby"};
        roomConnections["Operating Room"] = {"Room For Elderly"};
        roomConnections["Room For Elderly"] = {"Autopsy Room"};
        roomConnections["Autopsy Room"] = {"Room With Emergency Exit"};
        roomConnections["Lobby"] = {"Room With Emergency Exit", "Room With Xray Equipment"};
        roomConnections["Room With Xray Equipment"] = {"Room For Materials"};
        roomConnections["Room For Materials"] = {"Room With Emergency Exit"};
    }

    // functie care returneaza o camera dupa numele ei
    Room& FindRoomByName(const std::string& name) {
        for (auto& room : rooms) {
            if (room.GetRoomName() == name) {
                return room;
            }
        }
        throw std::runtime_error("Room not found: " + name);
    }

    // functia pentru logica jocului
    void StartGame() {
        try {
            // verific in primul rand daca a fost setat numele playerului
            if (player == nullptr) {
                std::cout << "    Player name is not set! Please set it first." << std::endl;
                return;
            }

            Game* game = new Game(*player, rooms, "Reception");
            std::cout << "           ===== The game has begun! Explore the room and escape! =====" << std::endl;
            std::cout << std::endl;
            std::cout << "        < Little tip: to fight the enemies you have to use the attack items \n"
                         "          that you will get by exploring the room or you will win them by \n"
                         "          solving puzzles. Otherwise, the enemies will kill you:( >" << std::endl;
            std::cout << std::endl;

            // logica intregului joc
            currentRoomName = "Reception"; // camera din care se incepe jocul este Reception
            bool escaped = false;
            while (!escaped) {
                DrawHospitalMap(currentRoomName);
                try {
                    Room& actual_room = FindRoomByName(currentRoomName);
                } catch (const std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
                Room& actual_room = FindRoomByName(currentRoomName);
                std::cout << std::endl;
                std::cout << "  -->    You are in room: " << actual_room.GetRoomName() << std::endl;
                actual_room.DisplayRoomInfo();

                int choice;
                std::cout << std::endl;
                std::cout << "      ============== OPTION: ================" << std::endl;
                std::cout << "      Choose an action:" << std::endl;
                std::cout << "      1. Explore Room (Collect Items)" << std::endl;
                std::cout << "      2. Fight Enemies" << std::endl;
                std::cout << "      3. Fight Special Enemies" << std::endl;
                std::cout << "      4. Solve the Puzzles" << std::endl;
                std::cout << "      5. View Inventory" << std::endl;
                std::cout << "      6. Unlock the door and advance to another room" << std::endl;
                std::cout << "      7. Exit Game" << std::endl;
                std::cout << "      =======================================" << std::endl;
                std::cout << "      Your choice: ";
                std::cin >> choice;

                switch (choice) {
                    case 1: {
                        // verific daca inventarul playerului este gol -> evit dublarea itemelor
                        for (int i = 0; i < actual_room.GetRoomItemCount(); i++) {
                            auto item = actual_room.GetRoomItem(i);
                            std::cout << "        " << i + 1 << ". Found item: " << item->GetItemName() << std::endl;
                            player->AddItemToInventory(std::shared_ptr<Item>(item));
                        }
                        std::cout << std::endl;
                        actual_room.ClearRoomItems();
                        break;
                    }
                    case 2: {
                        if (actual_room.GetRoomEntityCount() != 0) {
                            std::cout << "\n       You have been attacked by an enemy and you must fight him." << std::endl;
                            for (int i = 0; i < actual_room.GetRoomEntityCount(); i++) {
                                auto enemy = actual_room.GetRoomEntity(i);
                                std::cout << std::endl;
                                std::cout << "       Enemy encountered: " << enemy->GetEntityName() << std::endl;
                                Battle(*player, dynamic_cast<Enemy &>(*enemy));
                            }
                            if (player->GetCharacterHealth() >= 0) {
                                actual_room.ClearRoomEntity();
                            }
                        }
                        else {
                            std::cout << "      There are no enemies in the room to fight." << std::endl;
                        }
                        break;
                    }
                    case 3: {
                        if (actual_room.GetRoomSpecialEnemiesCount() != 0) {
                            std::cout << "\n       You have been attacked by an Special Enemy and you must fight him." << std::endl;
                            for (int i = 0; i < actual_room.GetRoomSpecialEnemiesCount(); i++) {
                                auto special_enemy = actual_room.GetRoomSpecialEnemies(i);
                                std::cout << std::endl;
                                std::cout << "       Special Enemy encountered: " << special_enemy->GetSpecialEnemyName() << std::endl;
                                BattleCH_SE(*player, *special_enemy);
                            }
                            if (player->GetCharacterHealth() >= 0) {
                                actual_room.ClearRoomSpecialEnemy();
                            }
                        }
                        else {
                            std::cout << "      There are no special enemies in the room to fight." << std::endl;
                        }
                        break;
                    }
                    case 4: {
                        int puzzle_choice;
                        std::cout << "      =============== PUZZLE TYPE =============" << std::endl;
                        std::cout << "      What type of puzzle do you want to solve? " << std::endl;
                        std::cout << "      1. Riddle" << std::endl;
                        std::cout << "      2. HangMan Game" << std::endl;
                        std::cout << "      =========================================" << std::endl;
                        std::cout << "      Your puzzle choice: ";
                        std::cin >> puzzle_choice;
                        switch (puzzle_choice) {
                            case 1: {
                                StartRiddleGame(*player);
                                break;
                            }
                            case 2: {
                                StartHangManGame(*player);
                                break;
                            }
                            default: {
                                std::cout << "      Invalid option for puzzle! Try again." << std::endl;
                                break;
                            }
                        }
                        break;
                    }
                    case 5: {
                        std::cout << std::endl;
                        std::cout << "         " << player->GetCharacterName() << " Inventory: " << std::endl;
                        player->DisplayInfoCharacter();
                        break;
                    }
                    case 6: {
                        // verific daca am ajuns in camera cu iesirea
                        if (currentRoomName == "Room With Emergency Exit") {
                            bool hasMasterKey = false;

                            // verfic daca player ul are master_key
                            for (int i = 0; i < player->GetInventorySize(); i++) {
                                if (player->GetCharacterInventoryItem(i)->GetItemName() == "Master Key") {
                                    hasMasterKey = true;
                                    break;
                                }
                            }

                            if (hasMasterKey) {
                                std::cout << "      ========================= YOU WON THE GAME ===================================" << std::endl;
                                std::cout << "      Congratulations! You found the master key and managed to unlock the exit door!" << std::endl;
                                std::cout << "      You have successfully escaped from the abandoned hospital!" << std::endl;
                                std::cout << "      " << player->GetCharacterName() << " score: " << player->GetCharacterScore() << std::endl;
                                std::cout << "      ==============================================================================" << std::endl;
                                escaped = true;
                                exit(1);
                                return;
                            } else {
                                std::cout << "      ============================== TRY AGAIN ======================================" << std::endl;
                                std::cout << "      You can't open the final door without the Master Key! You have to find the key!" << std::endl;
                                std::cout << "      ===============================================================================" << std::endl;
                                break;
                            }
                        }

                        // verific daca toate entitatile au fost invinse si toate itemele au fost colectate
                        Room& currentRoom = FindRoomByName(currentRoomName);

                        if (currentRoom.GetRoomEntityCount() > 0) {
                            std::cout << "      You can't leave the room! There are still enemies to defeat!" << std::endl;
                            break;
                        }
                        if (currentRoom.GetRoomSpecialEnemiesCount() > 0) {
                            std::cout << "      You can't leave the room! There are still special enemies to defeat!" << std::endl;
                            break;
                        }
                        if (currentRoom.GetRoomItemCount() > 0) {
                            std::cout << "      You can't leave the room! There are still items to collect!" << std::endl;
                            break;
                        }

                        // permit trecerea intre camere si afisez optiunile
                        auto connections = roomConnections[currentRoomName];
                        if (connections.empty()) {
                            std::cout << "      There are no connections from this room!" << std::endl;
                            break;
                        }

                        std::cout << "      You can go to the following rooms:" << std::endl;
                        for (int i = 0; i < connections.size(); i++) {
                            std::cout << "      " << (i + 1) << ". " << connections[i] << std::endl;
                        }

                        int roomChoice;
                        std::cout << "      Choose the room (1-" << connections.size() << "): ";
                        std::cin >> roomChoice;

                        if (roomChoice < 1 || roomChoice > connections.size()) {
                            std::cout << "      Invalid option! Try again." << std::endl;
                            break;
                        }

                        // schimb camera curenta
                        const std::string& nextRoom = connections[roomChoice - 1];

                        currentRoomName = nextRoom;
                        std::cout << "       You entered the room: " << currentRoomName << std::endl;
                        break;
                    }
                    case 7: {
                        std::cout << "       Exiting the game..." << std::endl;
                        escaped = true;
                        ExitGame();
                        break;
                    }
                    default: {
                        std::cout << "       Invalid option! Try again." << std::endl;
                        break;
                    }
                }
            }
        } catch (const GameException& e) {
            std::cout << "[ERROR]: " << e.what() << " (COD: " << e.GetErrorCode() << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "[STANDARD ERROR]: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "[UNKNOWN ERROR]" << std::endl;
        }
    }
};

int main() {
    Game game;
    game.ShowMenu();
    return 0;
}