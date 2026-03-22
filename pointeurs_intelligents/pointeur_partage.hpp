#pragma once

template <typename T>
class pointeur_partage;

//#############################################
//##            BLOC DE CONTROL              ##
//#############################################

template <typename T>
class bloc_de_controle
{
private:
	friend class pointeur_partage<T>;

	int strong_count;
	int weak_count;
	T* object_ptr;

	//###################################
	//#         Constructeurs           #
	//###################################

	//constructeurs
	bloc_de_controle(T* p) : object_ptr(p), strong_count(1), weak_count(1) {};

	//###################################
	//#          Destructeur            #
	//###################################

	//Destructeur du bloc et de l'objet pointé
	~bloc_de_controle()	{
		delete object_ptr;
	}


	//###################################
	//#        Méthodes d'accès         #
	//###################################

	//strong count
	int get_strong_count() const {
		return strong_count;
	}

	//weak count
	int get_weak_count() const {
		return (strong_count > 0) ? weak_count + 1 : weak_count;
	}

	//incrémentation du compteur de références fortes
	void increment_strong() {
		++strong_count;
		++weak_count;
	}

	//incrémentation du compteur de références faibles
	void increment_weak() {
		++weak_count;
	}

	//décrémentation du compteur de références fortes et déstruction de l'objet encapsulé si le compteur fort atteint zéro
	//on ne garde le bloc de control que si il reste des weak ptr, sinon on détruit aussi le bloc
	void decrement_strong() {
		--strong_count;
		--weak_count;

		if (strong_count == 0) {
			//libérer la ressource
			delete object_ptr;
			object_ptr = nullptr;

			if (weak_count == 0) {
				delete this;
			}
		}
	}

	//decrementation du compteur de références faibles et déstruction du bloc de contrôle si le compteur atteint zéro
	void decrement_weak() {
		--weak_count;
		if (weak_count == 0) {
			delete this;
		}
	}

	//Retourne le pointeur brut
	T* get_ptr() const {
		return (strong_count > 0) ? object_ptr : nullptr;
	}

};

//#############################################
//##            POINTEUR PARTAGE             ##
//#############################################

template <typename T>
class pointeur_partage
{
private:
	bloc_de_controle<T>* blc;
public:

	//###################################
	//#         Constructeurs           #
	//###################################

	//constructeur par défaut
	pointeur_partage() : blc(nullptr) {};

	//constructeur par pointeur classique
	pointeur_partage(T* ptr) {
		blc = new bloc_de_controle(ptr);
	}

	//Constructeur de copie incrémentant le compteur de références fortes
	pointeur_partage(const pointeur_partage& other) {
		blc = other.blc;
		blc->increment_strong;
	}

	//Constructeur par move
	pointeur_partage(pointeur_partage&& other) {
		blc = other.blc;
		other.blc = nullptr;
	}

	//Construction depuis unique_ptr

	
	//###################################
	//#         Affectations            #
	//###################################

	//affectation par copy
	pointeur_partage& operator=(pointeur_partage& other) {
		blc = other.blc;
		blc->increment_strong;
	}

	//Affectation par move
	pointeur_partage& operator=(pointeur_partage&& other) {
		blc = other.blc;
		other.blc = nullptr;
	}

	//###################################
	//#          Destructeur            #
	//###################################

	//destructeur
	~pointeur_partage() {
		blc->decrement_strong();
	}

	//###################################
	//#        Méthodes d'accès         #
	//###################################

	// Surcharge de l'opérateur de déréférencement
	T& operator*() {
		return (*(blc->get_ptr()));
	}

	//Surcharge de l'opérateur de flèche
	T* operator->() {
		return blc->get_ptr();
	}

	//retourne le nombre de référence fortes
	long use_count() {
		if (blc) return blc->get_strong_count();
		return 0;
	}

	//retourne true si le pointeur est unique
	bool unique() {
		if (blc) {
			if (blc->get_strong_count() > 0) {
				return false;
			}
			return true;
		}
		return false;
	}

	//Échange les pointeurs et blocs de deux pointeur_partagés
	void swap(pointeur_partage& other) {
		bloc_de_controle* temp = other.blc;
		blc = other.blc;
		other.blc = temp;
	}

	//Décrémente le compteur de strong et 
	void reset() {
		blc->decrement_strong();
		blc = nullptr;
	}

	//abandonne l'objet actuel et prend possession du nouveau
	void reset(T* p) {
		blc->decrement_strong();
		blc = new bloc_de_controle(p);
	}

	//surcharge de l'opérateur bool renvoie vrai si ptr != nullptr
	constexpr operator bool() {
		return (blc->get_ptr() != nullptr);
	}

};

//###################################
//#      Méthodes non membres       #
//###################################

//Fonction non membres pour créer un pointeur partagé
template <typename T>
pointeur_partage<T> rendre_partage(T* obj) {
	return pointeur_partage<T>(obj);
}
