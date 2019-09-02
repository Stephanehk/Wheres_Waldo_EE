// Este programa funciona para construir filtros com menos de 64Mbytes.
// Este programa funciona para elementos estruturantes com menos
//   de 255 elementos.
#include <ctime>
//#define range
#include <imgall>

//<<<<<<<<<<<<<<<<<<< rotinas de alocacao <<<<<<<<<<<<<<<<<<<<
void *aloc(int t)
{ void *p;
  if (t==0) return NULL;
  p=malloc(t);
  if (p==NULL) erro("Falta memoria no aloca");
  return p;
}

void *realoc(void *a, int t)
{ void *p;
  p=realloc(a,t);
  if (p==NULL) erro("Falta memoria no realoca");
  return p;
}

//<<<<<<<<<<<<<<<<<<<<<<<<<< EE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
struct VETW2 {
  WORD p[2];
  WORD &operator () (int c) {
    #ifdef range
    if (c<0 || 2<=c) erro("Indice invalida na VETW2");
    #endif
    return p[c];
  }
};

struct VETI2 {
  int p[2];
  int &operator () (int c) {
    #ifdef range
    if (c<0 || 2<=c) erro("Indice invalida na VETI2");
    #endif
    return p[c];
  }
};

class EE {
  public:
  VETI2 v[255-1]; //ordem linha,coluna
  int n;
  void aloca(int np) { n=np; }
  EE(void) { aloca(0); } // default constructor
  EE(int np) { aloca(np); }
  EE(EE &a); // copy constructor
  VETI2 &operator () (int i) {
    #ifdef range
    if (!(0<=i && i<n)) erro("EE indice fora");
    #endif
    return v[i];
  }
  int &operator () (int l, int c) {
    #ifdef range
    if (!(0<=l && l<n)) erro("EE indice fora");
    #endif
    return v[l](c);
  }
  void show(void);
};

/********************** classe ee *********************************/
EE::EE(EE &a)
// copy constructor
{ int i; aloca(a.n); for (i=0; i<n; i++) (*this)(i)=a(i); }

void leest(EE &ee, char *nome)
{ FILE *arq; int i,e;
  arq=fopen(nome,"r");
  if (arq==NULL) erro("The specified window does not exist");
  fscanf(arq,"%d",&(ee.n));
  if (ee.n>255-1) erro("Too large window");
  for (i=0; i<ee.n; i++) {
    e=fscanf(arq,"%d%d",&ee(i,0),&ee(i,1));
    if (e!=2) erro("Elemento estruturante 1");
  }
  e=fscanf(arq,"%d",&i);
  if (e!=EOF) erro("Elemento estruturante 2");
  fclose(arq);
}

void EE::show(void)
{ int i;
  printf("EE::show\n");
  for (i=0; i<n; i++)
    printf("i=%d l=%d c=%d\n",i,(*this)(i,0),(*this)(i,1));
}

void atribui(EE &ee, int n, ...)
{ int i; VETI2 *v;
  v=(VETI2 *)(&n+1);
  ee.aloca(n);
  for (i=0; i<n; i++) ee(i)=v[i];
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<< Classe FILT (cut-tree) <<<<<<<<<<<<<<<<<<<<<<<<
#define FOLHA 0xffffff
typedef struct { BYTE v; unsigned r:24; } NOFILT;
// e=indice do ee (plano de corte), se for no interno.
//   valor de saida, se for folha.
// r=indice da subarvore a direita.
//   a subarvore a esquerda sempre comeca na proxima posicao do vetor filt.
//   r=FOLHA indica folha.
// Obs: r de 24 bits consegue indexar 16 milhoes de NOFILT que ocupam 64 Mbytes.
//      Se o filtro gerado tiver mais de 64 Mbytes, e' melhor aumentar tamanho
//      de r para unsigned int e mudar valor de FOLHA.

class FILT {
 public:
  NOFILT *v;
  int t; // Total de NOFILTs alocados.
  int n; // Quantidade de NOFILTs utilizados no momento.
  NOFILT &operator () (int i) {
    #ifdef range
    if (!(0<=i && i<n)) erro("FILT indice fora");
    #endif
    return v[i];
  }
  void aloca(int tp) { t=tp; n=0; v=(NOFILT *)aloc(sizeof(NOFILT)*t); }
  FILT(void) { t=n=0; } // default constructor
  int insere(void) {
    if (n>=t) { t=t+t/2; v=(NOFILT *)realoc(v,sizeof(NOFILT)*t); }
    n++;
    return n-1;
  }
  //void impfilt(EE &ee, char *st);
  void lefilt(EE &ee, char *st);
};

void FILT::lefilt(EE &ee, char *st)
{ FILE *arq; int e;

  arq=fopen(st,"rb");
  if (arq==NULL) erro("The specified filter does not exist.");
  e=setvbuf(arq,NULL,_IOFBF,30000);
  if (e!=0) erro("Faltou memoria setvbuf");

  fread(&e,1,sizeof(e),arq); ee.aloca(e);
  fread(ee.v,ee.n,sizeof(ee.v[0]),arq);
  fread(&e,1,sizeof(e),arq); aloca(e);
  fread(v,e,sizeof(v[0]),arq);
  n=e;
  fclose(arq);
}

//<<<<<<<<<<< Variaveis globais <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
IMGBIN ent; IMGGRY sai; EE ee;
FILT filt;

BYTE entwee(VETW2 w, int ie)
{ return ent(w(0)+ee(ie,0),w(1)+ee(ie,1)); }

BYTE saiw(VETW2 w)
{ return sai(w(0),w(1)); }

//<<<<<<<<<<<< Acha/restaura <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
BYTE acha(int l, int c)
{ int i,ie; VETW2 w; BYTE b;

  w(0)=l; w(1)=c; i=0;
  while (filt(i).r!=FOLHA) {
    ie=filt(i).v;
    b=entwee(w,ie);
    if (b==0) i++; else i=filt(i).r;
  }
  return filt(i).v;
}

void restaura(void)
{ int l,c,v;
  int total,contador,umpct;

  contador=0; total=ent.nl()*ent.nc(); umpct=total/100;

  for (l=0; l<ent.nl(); l++)
    for (c=0; c<ent.nc(); c++) {

      if (contador%umpct==0) {
        fprintf(stderr,"%10.3f%% concluded...\r",100.0*contador/total);
      }
      contador++;

      v=acha(l,c);
      sai(l,c)=v;
    }
  fprintf(stderr,"%10.3f%% concluded...\r",100.0);
}

//<<<<<<<<<<<< main <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main(int argc, char **argv)
{ int t1,t2,t3,t4,i;

  if (argc!=4) {
    printf("AppI: Applies sequential decision-tree for inverse halftoning.\n");
    printf("AppI In-QX.BMP Filter.CTI Out-QP.TGA\n");
    printf("  In-QX.BMP = image to-be-inverse-halftoned (uncompressed BMP)\n");
    printf("  Filter.CTI = sequential decision-tree generated by BldI (CTI)\n");
    printf("  Out-QP.TGA = inverse-halftoned image (uncompressed TGA)\n");
    printf("Background is assumed to be white.\n");
    printf("This program cannot apply filters larger than 64 MBytes.\n");
    printf("This program cannot use window with more than 254 peepholes.\n");
    erro("Error: Invalid quantity of arguments.");
  }
  t1=time(NULL);

  le(ent,argv[1]); ent.backg()=1;
  sai.resize(ent.nl(),ent.nc()); sai.backg()=255;
  filt.lefilt(ee,argv[2]);
  t2=time(NULL);

  restaura();
  t3=time(NULL);

  sai.ImpTga(argv[3]);
  t4=time(NULL);

  printf("\nInverse halftoning is finished\n");
  for (i=0; i<argc; i++) printf("%s ",argv[i]);
  printf("\n");

  //printf("-------------------\n");
  //printf("Tamanho da imagem: linhas=%d colunas=%d pixels=%d\n",
  //       ent.nl(),ent.nc(),ent.nl()*ent.nc());
  //printf("-------------------\n");
  printf("Total processing time (s): %d\n",t4-t1);
  //printf("Tempo de processamento: %d\n",t3-t2);
  //printf("Tempo de leitura/escrita: %d\n",(t2-t1)+(t4-t3));
  printf("\n");
}

