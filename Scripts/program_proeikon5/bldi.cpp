// Este programa funciona para construir filtros com menos de 64Mbytes.
// Este programa funciona para elementos estruturantes com menos
//   de 255 elementos.
#include <cctype>
#include <ctime>
#include <cstring>
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
  if (arq==NULL) erro("Specified window does not exist");
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

//<<<<<<<<<<<<<<<<<<<<<<<< classe DAT <<<<<<<<<<<<<<<<<<<<<<<<<<
class DAT { // A ultima linha de dat e' dado temporario
 public:
  DWORD *x; BYTE *y; int nl,nc,ncw;
  void aloca(int nlp, int ncp);
  void realoca(int nlp);
  BYTE &sai(int l) {
    #ifdef range
    if (!(0<=l && l<nl)) erro("DAT indice fora");
    #endif
    return y[l];
  }
  DWORD &ent(int l, int cw) {
    #ifdef range
    if (!(0<=l && l<nl)) erro("DAT indice fora");
    if (!(0<=cw && cw<ncw)) erro("DAT indice fora");
    #endif
    return x[ncw*l+cw];
  }
  BYTE operator () (int l, int c) {
    #ifdef range
    if (!(0<=l && l<nl)) erro("DAT indice fora");
    if (!(0<=c && c<nc)) erro("DAT indice fora");
    #endif
    return (x[ncw*l+c/32] >> (c%32)) & 1;
  }
  void put(int l, int c, BYTE v) {
    #ifdef range
    if (!(0<=l && l<nl)) erro("DAT indice fora");
    if (!(0<=c && c<nc)) erro("DAT indice fora");
    #endif
    if (v==0) x[ncw*l+c/32] &= ~(1<<(c%32));
    else      x[ncw*l+c/32] |=  (1<<(c%32));
  }
  void show(void);
};

void DAT::aloca(int nlp, int ncp)
{ nl=nlp; nc=ncp; ncw=(nc+31)/32;
  x=(DWORD *)aloc(sizeof(DWORD)*nl*ncw);
  y=(BYTE *)aloc(sizeof(BYTE)*nl);
}

void DAT::realoca(int nlp)
{ nl=nlp;
  x=(DWORD *)realoc(x,sizeof(DWORD)*nl*ncw);
  if (x==NULL) erro("DAT::realoca");
  y=(BYTE *)realoc(y,sizeof(BYTE)*nl);
  if (y==NULL) erro("DAT::realoca");
}

/*
void DAT::show(void)
{ int l,c;

  for (l=0; l<nl; l++) {
    printf("%2d ",l);
    for (c=ncw-1; c>=0; c--) printf("%08X ",(*this).ent(l,c));
    for (c=nc-1; c>=0; c--) printf("%1d",(*this)(l,c));
    printf("-%03d\n",(*this).sai(l));
  }
  printf("\n");
}
*/

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
    #ifdef range
    if (t==0) erro("FILT nao foi alocado");
    #endif
    if (n>=t) { t=t+t/2; v=(NOFILT *)realoc(v,sizeof(NOFILT)*t); }
    n++;
    return n-1;
  }
  void impfilt(EE &ee, char *st);
  //void lefilt(EE &ee, char *st);
};

void FILT::impfilt(EE &ee, char *st)
{ FILE *arq; int e;

  arq=fopen(st,"wb");
  if (arq==NULL) erro("Erro na abertura de arquivo");
  e=setvbuf(arq,NULL,_IOFBF,30000);
  if (e!=0) erro("Faltou memoria setvbuf");

  fwrite(&(ee.n),1,sizeof(ee.n),arq);
  fwrite(ee.v,ee.n,sizeof(ee.v[0]),arq);
  fwrite(&n,1,sizeof(n),arq);
  fwrite(v,n,sizeof(v[0]),arq);
  fclose(arq);
}

//<<<<<<<<<<< Variaveis globais <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
IMGBIN ent;
IMGGRY sai;
IMGFLT mat;
EE ee,eo;
FILT filt; DAT dat;

BYTE entwee(int l, int c, int ie)
{ return ent(l+ee(ie,0),c+ee(ie,1)); }

BYTE saiw(int l, int c)
{ return sai(l,c); }

void showfilt(void)
{ int i;
  printf("****************** showfilt *********************\n");
  for (i=0; i<filt.n; i++) {
    printf("i=%d v=%d r=%d\n",i,filt(i).v,filt(i).r);
  }
  printf("\n");
}

//<<<<<<<<<<<<<<< ordena <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int comparadat(int a, int b)
{ int i;

  for (i=dat.ncw-1; i>=0; i--) {
    if (dat.ent(a,i)<dat.ent(b,i)) return -1;
    if (dat.ent(a,i)>dat.ent(b,i)) return 1;
  }
  if (dat.sai(a)<dat.sai(b)) return -1;
  if (dat.sai(a)>dat.sai(b)) return 1;
  return 0;
}

void atribuidat(int a, int b)
{ int i;

  for (i=dat.ncw-1; i>=0; i--) {
    dat.ent(a,i)=dat.ent(b,i);
  }
  dat.sai(a)=dat.sai(b);
}

void trocadat(int a, int b)
{ int i,w;

  for (i=dat.ncw-1; i>=0; i--) {
    w=dat.ent(a,i); dat.ent(a,i)=dat.ent(b,i); dat.ent(b,i)=w;
  }
  w=dat.sai(a); dat.sai(a)=dat.sai(b); dat.sai(b)=w;
}

void ordenadat(int l, int r)
{ int i,j; static int x;
  i=l; j=r; x=dat.nl-1; atribuidat(x,(l+r)/2);
  do {
    while (comparadat(i,x)<0) i++;
    while (comparadat(x,j)<0) j--;
    if (i<=j) { trocadat(i,j); i++; j--; }
  } while (i<=j);
  if (l<j) ordenadat(l,j);
  if (i<r) ordenadat(i,r);
}

//<<<<<<<<<<<<<<< moda <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int comparaent(int a, int b)
{ int i;

  for (i=dat.ncw-1; i>=0; i--) {
    if (dat.ent(a,i)<dat.ent(b,i)) return -1;
    if (dat.ent(a,i)>dat.ent(b,i)) return 1;
  }
  return 0;
}

void EscolheModa(void)
{ int l,r,inserir,moda,freqmax,freq,i;

  inserir=0;
  l=0; r=l;

  while (r<dat.nl-1) {
    r++;
    while (r<dat.nl-1 && comparaent(l,r)==0) r++;
    r--;

    // Processe l,r e ache moda
    moda=l; freqmax=1; freq=1;
    for (i=l+1; i<=r; i++) {
      if (dat.sai(i)!=dat.sai(i-1)) freq=1;
      else { freq++;
        if (freq>freqmax) { freqmax=freq; moda=i; }
      }
    }

    atribuidat(inserir,moda); inserir++;
    l=r+1; r=l;
  }
  dat.realoca(inserir);
}

void EscolheMedia(void)
{ int l,r,inserir;
  int i,soma;

  inserir=0;
  l=0; r=l;

  while (r<dat.nl-1) {
    r++;
    while (r<dat.nl-1 && comparaent(l,r)==0) r++;
    r--;

    // Processe l,r e ache media
    soma=0;
    for (i=l; i<=r; i++) soma+=dat.sai(i);
    dat.sai(l)=double2G(soma/(r-l+1.0));

    atribuidat(inserir,l); inserir++;
    l=r+1; r=l;
  }
  dat.realoca(inserir);
}

void EscolheBit(void)
{ int l,r,inserir;
  int i,j,soma[8];
  BYTE b;

  inserir=0;
  l=0; r=l;

  while (r<dat.nl-1) {
    r++;
    while (r<dat.nl-1 && comparaent(l,r)==0) r++;
    r--;

    // Processe l,r e ache media bit-a-bit
    for (j=0; j<8; j++) soma[j]=0;
    for (i=l; i<=r; i++) {
      b=dat.sai(i);
      for (j=0; j<8; j++) {
        soma[j]+=b & 1;
        b=b>>1;
      }
    }
    b=0;
    for (j=7; j>=0; j--) {
      b=b<<1;
      if (soma[j]>(r-l+1)/2) b=b+1;
    }
    dat.sai(l)=b;

    atribuidat(inserir,l); inserir++;
    l=r+1; r=l;
  }
  dat.realoca(inserir);
}

//<<<<<<<<<<<<<<< construcao <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int diferenca(int l, int r, int p)
{ int i,br,pt;
  br=pt=0;
  for (i=l; i<=r; i++) {
    if (dat(i,p)==0) pt++; else br++;
  }
  return abs(pt-br);
}

int EixoMelhorDistribuido(int l, int r, int pee)
{ int pc,mindif,dif,i;

  mindif=diferenca(l,r,eo(pee,0)); pc=pee;
  for (i=pee+1; i<eo.n; i++) {
    dif=diferenca(l,r,eo(i,0));
    if (dif<mindif) { mindif=dif; pc=i; }
  }
  return pc;
}

int partition(int l, int r, int ie)
//l..m-1:0; m..r:1 (m valor retornado)
{
  do {
    while (l<=r && dat(l,ie)==0) l++; // A esquerda de l so tem 0
    while (l<=r && dat(r,ie)==1) r--; // A direita de r so tem 1
    if (l<=r) { // Aqui, l nunca pode ser igual a r
      trocadat(l,r);
      l++; r--;
    }
  } while (l<=r);
  return l;
}

int SaidasIguais(int l, int r)
{ int i; BYTE b;
  b=dat.sai(l);
  for (i=l+1; i<=r; i++)
    if (dat.sai(i)!=b) return 0;
  return 1;
}

void trocaee(VETI2 &x, VETI2 &y)
{ VETI2 t;
  t=x; x=y; y=t;
}

void deslocaeo(int pee, int pc)
//pee<=pc
{ VETI2 t; int i;
  if (pee<pc) {
    t=eo(pee); eo(pee)=eo(pc);
    for (i=pc-1; i>pee; i--) eo(i+1)=eo(i);
    eo(pee+1)=t;
  }
}

void invdeslocaeo(int pee, int pc)
//pee<=pc
{ VETI2 t; int i;
  if (pee<pc) {
    t=eo(pee+1);
    for (i=pee+1; i<pc; i++) eo(i)=eo(i+1);
    eo(pc)=eo(pee); eo(pee)=t;
  }
}

void construcao(int l, int r, int pf, int pee)
// pf=indice de de elemento do filt vazio e previamente alocado.
// pee=indice do ee a partir do qual deve processar.
{ int pc,m,p;

  if (l==r || SaidasIguais(l,r)) { //Base da recursão
    filt(pf).v=dat.sai(l);
    filt(pf).r=FOLHA;
  } else {

    #ifdef range
    if (pee>=ee.n) erro("Isto nao deveria acontecer!");
    #endif

    pc=EixoMelhorDistribuido(l,r,pee); //pc=plano de corte
    filt(pf).v=eo(pc,0);

    //trocaee(eo(pc),eo(pee));
    deslocaeo(pee,pc);
    m=partition(l,r,eo(pee,0)); //l..m-1:0; m..r:1

    #ifdef range
    eo(pee,1)=0;
    #endif
    if (l<=m-1) construcao(l,m-1,filt.insere(),pee+1);

    #ifdef range
    eo(pee,1)=1;
    #endif
    p=filt.insere(); filt(pf).r=p;

    if (m<=r) construcao(m,r,p,pee+1);
    //trocaee(eo(pc),eo(pee));
    invdeslocaeo(pee,pc);
  }
}

//<<<<<<<<<<<<<<<<<< ordenaee <<<<<<<<<<<<<<<<<<<<<<<<<<
int comparaee(VETI2 a, VETI2 b)
{ int da,db;

  da=a(0)*a(0)+a(1)*a(1);
  db=b(0)*b(0)+b(1)*b(1);

  if (da<db) return -1;
  if (da>db) return 1;
  return 0;
}

void ordenaee(int l, int r)
{ int i,j; VETI2 x;
  i=l; j=r; x=ee((l+r)/2);
  do {
    while (comparaee(ee(i),x)<0) i++;
    while (comparaee(x,ee(j))<0) j--;
    if (i<=j) { trocaee(ee(i),ee(j)); i++; j--; }
  } while (i<=j);
  if (l<j) ordenaee(l,j);
  if (i<r) ordenaee(i,r);
}

//<<<<<<<<<<<<<<<<<<<< main <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main(int argc, char **argv)
{ int l,c,nl,nc,i,t1,t2,t3,t4,t5,t6,t7;
  int modo; //0=av 1=mo 2=bi

  if (argc!=6 && argc!=5) {
    printf("BldI: Builds sequential decision-tree for inverse halftoning.\n");
    printf("BldI In-AX.BMP Out-AY.TGA E.MAT Filter.CTI [<<av>> mo bi]\n");
    printf("  In-AX.BMP = input sample halftone image (uncompressed BMP)\n");
    printf("  Out-AY.TGA = output sample grayscale image (uncompressed TGA)\n");
    printf("  E.MAT = window definition (TXT)\n");
    printf("  Filter.CTI = sequential decision-tree to be generated by BldI (CTI)\n");
    printf("  Operation mode: AV:average(default) MO:mode BI:bitwise-average\n");
    printf("Background is assumed to be white.\n");
    printf("This program cannot build filters larger than 64 MBytes.\n");
    printf("This program cannot use window with more than 254 peepholes.\n");
    erro("Error: Invalid quantity of arguments.");
  }
  printf("Allocating memory...\n");

  t1=time(NULL);
  modo=0;
  if (argc==6) {
    strlwr(argv[5]);
    if (strcmp(argv[5],"av")==0) modo=0;
    else if (strcmp(argv[5],"mo")==0) modo=1;
    else if (strcmp(argv[5],"bi")==0) modo=2;
    else erro("unknown operation mode");
  }
  le(ent,argv[1]); ent.backg()=1;
  le(sai,argv[2]); sai.backg()=255;
  le(mat,argv[3]);

  t2=time(NULL);
  for (l=0; l<mat.nl(); l++)
    for (c=0; c<mat.nc(); c++)
      if (mat(l,c)!=0) {
        ee.n++; ee(ee.n-1,0)=l-mat.lc(); ee(ee.n-1,1)=c-mat.cc();
      }
  ordenaee(0,ee.n-1);

  eo.aloca(ee.n);
  for (l=0; l<ee.n; l++) eo(l,0)=l;

  if (ent.nc()!=sai.nc() || ent.nl()!=sai.nl())
    erro("Error: Two images must have the same dimensions.");
  nc=ent.nc(); nl=ent.nl();

  dat.aloca(nl*nc+1,ee.n); // A ultima linha de dat e' dado temporario
  for (l=0; l<nl; l++)
    for (c=0; c<nc; c++) {
      dat.ent(l*nc+c,dat.ncw-1)=0;
      for (i=0; i<ee.n; i++) dat.put(l*nc+c,i,entwee(l,c,i));
      dat.sai(l*nc+c)=saiw(l,c);
    }
  ent.resize(0,0); sai.resize(0,0);

  t3=time(NULL);
  printf("Sorting...\n");
  ordenadat(0,dat.nl-2);

  //Antes de EscolheModa, dat tem uma linha a mais para temporario.
  t4=time(NULL);
  printf("Choosing av/mo/bi...\n");
  if (modo==0) EscolheMedia();
  else if (modo==1) EscolheModa();
  else if (modo==2) EscolheBit();
  else erro("Isto nao deveria acontecer");
  //Apos EscolheModa, dat numero de linhas correto.

  t5=time(NULL);
  printf("Building DT...\n");
  filt.aloca(2*dat.nl);
  construcao(0,dat.nl-1,filt.insere(),0);

  t6=time(NULL);
  printf("Writing DT...\n");
  filt.impfilt(ee,argv[4]);
  t7=time(NULL);

  printf("-------------------\n");
  for (i=0; i<argc; i++) printf("%s ",argv[i]);
  printf("\n");
  //printf("Tamanho da imagem: linhas=%d colunas=%d pixels=%d\n",nl,nc,nl*nc);
  //printf("Exemplos apos eliminar padroes repetidos=%d\n",dat.nl);
  //printf("Exemplos repetidos sem conflito=%d\n",pixelsrep);
  //printf("Exemplos repetidos com conflito=%d\n",pixelsconf);
  //printf("Soma das tres linhas anteriores=%d. Igual a pixels.\n",
  //       pixelsolit+pixelsrep+pixelsconf);
  //printf("-------------------\n");
  //printf("Number of nodes=%d.\n",filt.n);
  //printf("Folhas=%d Nos internos=%d.\n",folhas,filt.tf-folhas);
  //printf("ExempPos=%d. ExempNeg=%d. Total exemplos=%d (igual a folhas).\n",
  //       exemppos,exempneg,exemppos+exempneg);
  //printf("Conflitos=%d. Repeticoes=%d. ExemplosSemRep=%d.\n",
  //        conflitos,repeticoes,conflitos+repeticoes+pixelsolit);

  //printf("-------------------\n");
  printf("Total processing time (s): %d\n",t7-t1);
  //printf("  Tempo de processamento: %d\n",t6-t2);
  //printf("    Tempo de inicializacao: %d\n",t3-t2);
  //printf("    Tempo de ordenacao: %d\n",t4-t3);
  //printf("    Tempo de calculo de moda: %d\n",t5-t4);
  //printf("    Tempo de construcao da arvore: %d\n",t6-t5);
  //printf("  Tempo de leitura/escrita: %d\n",(t2-t1)+(t7-t6));
  printf("\n");
}

