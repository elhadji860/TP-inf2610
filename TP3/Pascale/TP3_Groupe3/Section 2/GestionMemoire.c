#include<stdbool.h>
#include "./libs/lib.h"

unsigned long taillePage = 1024;

unsigned int calculerNumeroDePage(unsigned long adresse) {
	// TODO
    return (adresse >> 10);
}

unsigned long calculerDeplacementDansLaPage(unsigned long adresse) {
	// TODO
     return adresse % taillePage;
}

unsigned long calculerAdresseComplete(unsigned int numeroDePage, unsigned long deplacementDansLaPage) {
	// TODO
     return (numeroDePage * taillePage) + deplacementDansLaPage;
}

void rechercherTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	// TODO
	unsigned int numeroPage = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);

    struct TLB* tlb = mem->tlb;

    for (int i = 0; i < TAILLE_TLB; i++) {
        if (tlb->entreeValide[i] && tlb->numeroPage[i] == numeroPage) {
            req->estDansTLB = 1; 
            req->adressePhysique = calculerAdresseComplete(tlb->numeroCadre[i], deplacement);
            tlb->dernierAcces[i] = req->date;
            return; 
        }
    }

    req->estDansTLB = 0; 
    req->adressePhysique = 0;
}

void rechercherTableDesPages(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	// TODO
	unsigned int numeroPage = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);

    struct TablePages* tp = mem->tp;

    if (tp->entreeValide[numeroPage]) {
        req->estDansTablePages = 1;
        req->adressePhysique = calculerAdresseComplete(tp->numeroCadre[numeroPage], deplacement);
    } else {
        req->estDansTablePages = 0;
        req->adressePhysique = 0;
    }
}

void ajouterDansMemoire(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	//TODO
	unsigned int numeroPage = calculerNumeroDePage(req->adresseVirtuelle);

    struct Memoire* memoire = mem->memoire;
    struct TablePages* tp = mem->tp;

    for (int i = 0; i < TAILLE_MEMOIRE; i++) {
        if (!memoire->utilisee[i]) { 
            memoire->numeroPage[i] = numeroPage;
            memoire->utilisee[i] = 1;
            memoire->dateCreation[i] = req->date;
            memoire->dernierAcces[i] = req->date;

            tp->numeroCadre[numeroPage] = i; 
            tp->entreeValide[numeroPage] = 1; 
            unsigned long deplacement = calculerDeplacementDansLaPage(req->adresseVirtuelle);
            req->adressePhysique = calculerAdresseComplete(i, deplacement);
            req->estDansTablePages = 1;

            return;
        }
    }
}

void mettreAJourTLB(struct RequeteMemoire* req, struct SystemeMemoire* mem) {
	// TODO
	struct TLB* tlb = mem->tlb;
	struct TablePages* tp = mem->tp;

    int indiceRemplacement = 0;
    unsigned long plusAncienneDate = tlb->dateCreation[0];

    for (int i = 1; i < TAILLE_TLB; i++) {
        if (tlb->entreeValide[i] && tlb->dateCreation[i] < plusAncienneDate) {
            plusAncienneDate = tlb->dateCreation[i];
            indiceRemplacement = i;
        }
    }

    if (!tlb->entreeValide[indiceRemplacement]) {
        indiceRemplacement = 0;
    }

    unsigned int numeroPage = calculerNumeroDePage(req->adresseVirtuelle);
    unsigned int numeroCadre = tp->numeroCadre[numeroPage];
    tlb->numeroPage[indiceRemplacement] = numeroPage; 
    tlb->numeroCadre[indiceRemplacement] = numeroCadre;
    tlb->entreeValide[indiceRemplacement] = 1;
    tlb->dateCreation[indiceRemplacement] = req->date;
    tlb->dernierAcces[indiceRemplacement] = req->date;
}

// NE PAS MODIFIER
int main() {
    evaluate(
		&calculerNumeroDePage, 
		&calculerDeplacementDansLaPage, 
		&calculerAdresseComplete, 
        &rechercherTLB, 
		&rechercherTableDesPages,
		&mettreAJourTLB,
		&ajouterDansMemoire
    );
    return 0;
}
