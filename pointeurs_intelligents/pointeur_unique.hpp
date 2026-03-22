#pragma once

//#############################################
//##            POINTEUR UNIQUE              ##
//#############################################

template <typename T>
class pointeur_unique
{
private:
	T* ptr;
public:


	/*
	
	/!\ Ce qui n’existe pas

	Pas de constructeur unique_ptr(T&) : une référence ne transporte pas la propriété.
	Pas de constructeur depuis un shared_ptr (les modèles de propriété diffèrent).
	Pas de copie (unique_ptr est move-only).
	Pour les tableaux, pas de constructeur qui prenne la taille : 
	elle n’est pas stockée (utilise std::make_unique<T[]>(n) pour créer et gérer l’allocation).

	*/ 

	//###################################
	//#         Constructeurs           #
	//###################################

	//Constructeur par défaut
	pointeur_unique() : ptr(nullptr) {};

	//Constructeur par move
	pointeur_unique(pointeur_unique<T>&& u) noexcept {
		ptr = u.release();
	}

	//Constructeur par pointeur classique
	pointeur_unique(T* t) noexcept {
		ptr = t;
	}

	//On interdit la construction par copie
	pointeur_unique(const pointeur_unique&) = delete;

	//###################################
	//#         Affectations            # 
	//###################################

	//affectation par copy interdite
	pointeur_unique& operator=(const pointeur_unique&) = delete;

	//affectation par move
	pointeur_unique& operator=(pointeur_unique&& other) {
		ptr = other.release();
		return *this;
	}

	//###################################
	//#          Destructeur            #
	//###################################

	//Destructeur 
	~pointeur_unique(){
		delete ptr;
	}

	//###################################
	//#        Méthodes d'accès         #
	//###################################

	// Surcharge de l'opérateur de déréférencement
	T& operator*() {
		return (*ptr);
	}

	//Surcharge de l'opérateur de flèche
	T* operator->() {
		return ptr;
	}

	//Libère la ressource et retourne le pointeur brut
	T* release() {
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}

	//Retourne le pointeur brut sans libérer la ressource
	T* get() const {
		return ptr;
	}

	//Libère la ressource et remet le pointeur à nullptr
	void reset() {
		delete ptr;
		ptr = nullptr;
	}

	//Libère la ressource et prend possession d'un nouveau pointeur
	void reset(T* p) {
		delete ptr;
		ptr = p;
	}

	//Échange les pointeurs de deux pointeur_unique
	void swap(pointeur_unique& other) {
		T* temp = ptr;
		ptr = other.ptr;
		other.ptr = temp;
	}

	//surcharge de l'opérateur bool renvoie vrai si ptr != nullptr
	constexpr operator bool() {
		return (ptr != nullptr);
	}

	
};

//###################################
//#      Méthodes non membres       #
//###################################

//Fabrique
template <typename T>
pointeur_unique<T> rendre_unique(T& t) {
	return pointeur_unique<T>(t);
};



