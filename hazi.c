#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum kor { ifjusagi, junior } kor; //a k�t koroszt�ly defini�l�sa

//sportol� strukt�r�ja, minden sz�ks�ges adattal
typedef struct sportolo {
	char nev[30];
	int ev;
	double csop;
	kor k;
	double tomeg;
	double gugg;
	double nyom;
	double felh;
	double sum;
	double w;
	struct sportolo *next;
} sportolo;

//egy koroszt�ly adott s�lycsoportj�nak rekordjai
typedef struct rekord {
	kor k;
	double csop;
	double gugg;
	double nyom;
	double felh;
	double sum;
	struct rekord *next;
} rekord;

//a s�lycsoport meg�llap�t�s�hoz haszn�lt f�ggv�ny seg�df�ggv�nye
//akkor m�dos�tja a sportol� s�lycsoportj�t, ha a sportol� az adott s�lycsoportba tartozik, egy�bk�nt v�ltozatlanul hagyja
void segcsop(sportolo *p, double ah, double fh) {
	if (p->tomeg <= fh && p->tomeg > ah)
		p->csop = fh;
}

//a seg�df�ggv�ny futtat�sa minden s�lycsoportra
void sulycsoport(sportolo *p) {
	segcsop(p, 0.0, 53.0);
	segcsop(p, 53.0, 59.0);
	segcsop(p, 59.0, 66.0);
	segcsop(p, 66.0, 74.0);
	segcsop(p, 74.0, 83.0);
	segcsop(p, 83.0, 93.0);
	segcsop(p, 93.0, 105.0);
	segcsop(p, 105.0, 120.0);
	if (p->csop == 0.0)
		p->csop = 121.0;
}

//a koroszt�ly meghat�roz�sa a sportol� sz�let�si �v�b�l
void korosztaly(sportolo* h) {
	sportolo* p = h;
	if (p->ev >= 2000)
		p->k = ifjusagi;
	else {
		if (p->ev <= 1999 && p->ev >= 1995)
			p->k = junior;
		else {
			printf("kilogo adat"); return -1;
		}
	}
}

//Wilks-formula f�ggv�nye
double wilks(double t, double total) {
	double a = -216.0475144, b = 16.2606339, c = -0.002388645, d = -0.00113732, e = 0.00000701863, f = -0.00000001291;
	double w;
	w = total * 500 / (a + b * t + c * t*t + d * t*t*t + e * t*t*t*t + f * t*t*t*t*t);
	return w;
}

//A sportol� vezet�knev�t �s keresztnev�t f�zi �ssze, hogy egy sztringk�nt t�rolhassuk
char *osszefuz(char c1[], char c2[]) {
	int i;
	for (i = 0; c1[i] != '\0'; i++);
	c1[i] = ' '; c1[i + 1] = '\0';
	strcat(c1, c2);
	return c1;
}

//a rekordok beolvas�sakor haszn�lt f�ggv�ny a sztringet �rtelmezi, �s meg�llap�tja hogy a rekordok melyik koroszt�lyhoz tartoznak
kor atir(char str[]) {
	char j[] = "junior"; int c;
	c=strcmp(j, str);
	if (c == 0)
		return junior;
	else return ifjusagi;
}

//a sportol�k adatainak beolvas�sa f�jlb�l, illetve l�ncolt list�ba f�z�se
void read_sportolo(sportolo * head, FILE* e) {
	int ev; double tomeg, gugg, nyom, felh; char c2[15] = "/0"; sportolo* uj;
	sportolo* p = (sportolo *)malloc(sizeof(sportolo));
	p = head;
	//addig olvas, am�g tud
	while (fscanf(e, "%d %lf %lf %lf %lf", &ev, &tomeg, &gugg, &nyom, &felh) == 5 && p != NULL && p->next != NULL) {
		p->ev = ev; p->tomeg = tomeg; p->gugg = gugg; p->nyom = nyom; p->felh = felh;
		fscanf(e, "%s %s", p->nev, c2);
		osszefuz(p->nev, c2); //n�v meg�llap�t�sa
		p->sum = p->gugg + p->nyom + p->felh; //�sszetett eredm�ny kisz�m�t�sa
		p->w = wilks(p->tomeg, p->sum); //Wilks-pontsz�m kisz�mol�sa
		korosztaly(p);  //koroszt�ly meg�llap�t�sa f�ggv�nnyel
		p->csop = 0.0; //a sulycsoport f�ggv�ny megh�v�sa el�tt �rt�ket kell adni a p->csop-nak
		sulycsoport(p);
		uj = (sportolo*)malloc(sizeof(sportolo)); //k�vetkez� listaelem
		p->next = uj;
		p = p->next;
	}
	p->next = NULL; //az utols� listaelem next pointer�nek �rt�kad�sa
}

//rekordok beolvas�sa �s l�ncolt list�ba f�z�se
void read_rekord(rekord* head, FILE* f) {
	char str[9] = "\0"; //a koroszt�lyok beolvas�s�hoz haszn�lt sztring
	double cs, gugg, nyom, felh, sum; rekord *uj;
	rekord* p = (rekord *)malloc(sizeof(rekord));
	p = head;
	while (fscanf(f, "%s %lf %lf %lf %lf %lf", str, &cs, &gugg, &nyom, &felh, &sum) == 6 && p != NULL && p->next != NULL){
		p->k = atir(str);
		p->csop = cs; p->gugg = gugg; p->nyom = nyom; p->felh=felh; p->sum = sum;
		uj = (rekord *)malloc(sizeof(sportolo));
		p->next = uj;
		p = p->next;
	}
	p->next = NULL;
}

//a diszkvalifik�lt versenyz�k nev�t ki�r� f�ggv�ny
void diszkvalifikalt(sportolo* head){
	sportolo* p = head; int db = 0;
	printf("Diszkvalifikalt versenyzo(k): \n");
	while (p != NULL) {
		if (p->sum == 0.0) {
			db++;
			printf("%s \n\n", p->nev);
		}
		p = p->next;
	}
	if (db == 0) //nem biztos, hogy van diszkvalifik�lt versenyz�
		printf("Nincs diszkvalifikalt versenyzo \n\n");
}

//egy adott csoport gy�ztes�t ki�r� f�ggv�ny
void csoportgyoztes(sportolo* head, kor k, double suly) {
	if (suly != 53.0 && suly != 59.0 && suly != 66.0 && suly != 74.0 && suly != 83.0 && suly != 93.0 && suly != 105.0 && suly != 120.0 && suly != 121.0)
		printf("\n Nincs ilyen sulycsoport \n"); //ellen�rzi a kapott adat helyess�g�t
	sportolo* p = head;
	double max = 0.0;
	char gyoztes[30] = "Nincs gyoztes"; //a gy�ztes nev�t t�rol� sztring; ha �res a csoport, vagy csak diszkvalif�lt versenyz�k vannak benne, akkor nincs gy�ztes
	while (p != NULL) {
		if (p->k == k && p->csop == suly && p->sum > max && p->sum!=0) {
			strcpy(gyoztes, p->nev);
			max = p->sum; //megkeresi �s t�rolja a legjobb eredm�nyt
		}
		p = p->next;
	}
	if (k == ifjusagi)
		printf("ifjusagi \t"); //ki�rja a koroszt�lyt is
	else printf("junior \t");
	if (max!=0.0)
		printf("%.0f \t %s \t %.2f \n", suly, gyoztes, max);
	else printf("%.0f \t %s \n", suly, gyoztes);

}

//megh�vja acsoportgyoztes f�ggv�nyt minden koroszt�lyra �s s�lycsoportra
void csoportgyoztesek(sportolo* head) {
	int i, j;
	kor k[2] = { ifjusagi, junior };
	double suly[9] = { 53.0, 59.0, 66.0, 74.0, 83.0, 93.0, 105.0, 120.0, 121.0 };
	printf("csoportgyoztesek: \n\n");
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 9; j++) {
			csoportgyoztes(head, k[i], suly[j]);
		}
	}
}

//a legnagyobb Wilks-pontsz�mmal rendelkez� versenyz� nev�t ki�r� f�ggv�ny
void abszolut(sportolo* head) {
	sportolo* p = head;
	double maxw;
	char gyoztes[30] = "\0";
	strcpy(gyoztes, p->nev);
	maxw = p->w; //a maxw el�sz�r az els� sportol� pontsz�m�nak �rt�k�t veszi fel
	while (p != NULL) {
		if (p->w > maxw) {
			maxw = p->w;
			strcpy(gyoztes, p->nev);
		}
		p = p->next;
	}
	printf("Az abszolut gyoztes: \t %s\n\n", gyoztes);
}

//az �j rekordokat ki�r� f�ggv�ny
void ujrekord(rekord* headr, sportolo*heads) {
	sportolo* p = heads;
	rekord* r = headr;
	int db = 0;
	/*a f�ggv�ny a rekordok lista egy elem�re megvizsg�lja a sportol�k list�j�nak elemeit,
	ezt megism�tli a rekordok lista minden elem�re;
	a sportol� eredm�nyeit csak akkor hasonl�tja �ssze a rekorddal, hogy ha a koroszt�ly �s a s�lycsoport egyezik*/
	while (r != NULL) {
		while (p != NULL) {
			if (p->csop == r->csop && p->k == r->k && p->gugg > r->gugg) {
				db++;
				printf("Uj guggolas rekord:\t %s \t %.2f kg \n", p->nev, p->gugg);
			}
			if (p->csop == r->csop && p->k == r->k && p->nyom > r->nyom) {
				db++;
				printf("Uj fekvenyomas rekord:\t %s \t %.2f kg \n", p->nev, p->nyom);
			}
			if (p->csop == r->csop && p->k == r->k && p->felh > r->felh) {
				db++;
				printf("Uj felhuzas rekord:\t %s \t %.2f kg \n", p->nev, p->felh);
			}
			if (p->csop == r->csop && p->k == r->k && p->sum > r->sum) {
				db++;
				printf("Uj osszetett rekord:\t %s \t %.2f kg \n", p->nev, p->sum);
			}
			p = p->next;
		}
		r = r->next;
		p = heads;
	}
	printf("Osszesen %d db uj rekord szuletett\n", db);
}

//a list�k elej�r�l t�rl� f�ggv�nyek
sportolo *pop_front_sportolo(sportolo* head) {
	sportolo *p = head;
	if (head != NULL) {
		head = head->next;
		free(p);
	}
	return head;
}

rekord *pop_front_rekord(rekord* head) {
	rekord *p = head;
	if (head != NULL) {
		head = head->next;
		free(p);
	}
	return head;
}

//a list�k teljes t�rl�se a lista elej�r�l t�rl� f�ggv�nyek ism�tl�s�vel
void sportolo_listatorles(sportolo* head) {
	while (head != NULL) {
		head=pop_front_sportolo(head);
	}
}

void rekord_listatorles(rekord* head) {
	while (head != NULL) {
		head=pop_front_rekord(head);
	}
}

int main() {
	unsigned opcio, korosztaly;double suly; kor k;
	//forr�sf�jlok megnyit�sa �s l�ncolt list�k l�trehoz�sa
	FILE *e = fopen("eredmenyek.txt", "r");
	if (e == NULL) {
		printf("eredmenyek.txt megnyitasa sikertelen"); return -1;
	}
	sportolo* p = (sportolo *)malloc(sizeof(sportolo));
	read_sportolo(p, e);

	FILE *f = fopen("rekordok.txt", "r");
	if (f == NULL) {
		printf("rekordok.txt megnyitasa sikertelen"); return -1;
	}
	rekord* r = (rekord *)malloc(sizeof(rekord));
	read_rekord(r, f);

	//a main egy men�rendszerrel m�k�dik, a felhaszn�l� v�lasztja ki a haszn�lni k�v�nt funkci�t
	printf("\n Mire vagy kivancsi? \n\n 1: diszkvalifikalt versenyzo(k)\n 2: adott csoport gyoztese \n 3: osszes csoportgyoztes\n 4: abszolut gyoztes \n 5: uj rekordok\n 0: bezaras \n \n");
	scanf("%d", &opcio);
	//a program addig fut, am�g a felhaszn�l� nem v�lasztja ki a bez�r�s opci�t
	//a men�rendszer megval�s�t�sa egy while ciklusba �gyazott switch-case funkci�val
	while (opcio != 0) {
		switch (opcio) {
		case (1):
			diszkvalifikalt(p); break;
		case (2):
			//a felhaszn�l� v�lasztja ki a koroszt�lyt; a program vizsg�lja a megadott adat helyess�g�t
			printf("Valassz korosztalyt!\n 1: ifjusagi \n 2: junior\n ");
			scanf("%d", &korosztaly);
			if (korosztaly == 1)
				k = ifjusagi;
			else if (korosztaly == 2)
				k = junior;
			else{
				printf("Nincs ilyen korosztaly");
				break;
				}
			printf("Add meg a sulycsoportot!\n(53, 59, 66, 74, 83, 93, 105, 120, 121)\n");
			scanf("%lf", &suly);
			csoportgyoztes(p, k, suly); break;
		case (3):
			csoportgyoztesek(p); break;
		case (4):
			abszolut(p); break;
		case (5):
			ujrekord(r, p); break;
		default: printf("Nincs ilyen opcio!");
		//a program jelez ha a felhaszn�l� nem l�tez� opci�val pr�b�lkozik
		}
		printf("\nMire vagy kivancsi? \n\n 1: diszkvalifikalt versenyzo(k)\n 2: adott csoport gyoztese \n 3: osszes csoportgyoztes\n 4: abszolut gyoztes \n 5: uj rekordok\n 0: bezaras \n \n");
		scanf("%d", &opcio);
	}
	//f�jlok bez�r�sa �s list�k t�rl�se
	fclose(e);
	fclose(f);
	sportolo_listatorles(p);
	rekord_listatorles(r);
	return 0;
}
