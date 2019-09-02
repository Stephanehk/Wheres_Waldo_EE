#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <imgall>

//typedef VET2<int> PONTOI2;
typedef VETOR<PONTOI2> EE;

//#define range

const int FIM=0xffffffff;
int nc,nl;
IMGGRY ent,sai;
int folha=0;
//double somadesvio=0.0;
char modoconf;

EE ee; //ordem linha,coluna - tem que inicializar
int nnosfolha;

void leest(char *nome)
{ FILE *arq; int i;
  arq=fopen(nome,"r");
  if (arq==NULL) erro("Specified window does not exist");
  int nee;
  fscanf(arq," %d ",&nee);
  ee.resize(nee);
  for (i=0; i<nee; i++) {
    fscanf(arq," %d %d ",&ee(i)(0),&ee(i)(1));
  }
  fclose(arq);
}

//<<<<<<<<<<<<<<<<<<<<<<<<< ind <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
class IND {
 public:
  PONTOI2 *v; int ni;
  PONTOI2 &operator [] (int i)
    {
      #ifdef range
      if (!(0<=i && i<ni)) erro("IND indice fora");
      #endif
      return v[i];
    }
  void aloc(int n) { ni=n; v=(PONTOI2 *)aloca(sizeof(PONTOI2)*ni); }
};

IND ind;

//<<<<<<<<<<<<<<<< filt <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
typedef struct { int e,l,r; } NOFILT;
// e=indice do ee.
//   Valor 0xffffffff (FIM) indica folha da arvore.
// l=indice da subarvore a esquerda.
//   Quando for folha, valor da media.
// r=indice da subarvore a direita.
class FILT {
 public:
  NOFILT *v; int nf,tf;
  NOFILT &operator [] (int i)
    {
      #ifdef range
      if (!(0<=i && i<nf)) erro("FILT indice fora");
      #endif
      return v[i];
    }
  void operator = (NOFILT *p) { v=p; }
  void aloc(int n) { nf=n; tf=0; v=(NOFILT *)aloca(sizeof(NOFILT)*nf); }
  FILT(void) { v=0; nf=tf=0; }
  int insere(int ep, int lp, int rp);
  void impfilt(char *st);
  void lefilt(char *st);
};
FILT filt;

int FILT::insere(int ep, int lp, int rp)
// Usado para inserir no interno e folha
{ if (tf>=nf) { nf=nf+nf/2; v=(NOFILT *)realoca(v,sizeof(NOFILT)*nf); }
  v[tf].e=ep; v[tf].l=lp; v[tf].r=rp;
  tf++; return tf-1;
}

void FILT::impfilt(char *st)
{ FILE *arq; int e;

  arq=fopen(st,"wb");
  if (arq==NULL) erro("Erro na abertura de arquivo");
  e=setvbuf(arq,NULL,_IOFBF,0x10000);
  if (e!=0) erro("Faltou memoria setvbuf");

  e=ee.n(); fwrite(&e,1,sizeof(e),arq);
  fwrite(ee.vet,ee.n(),sizeof(PONTOI2),arq);
  fwrite(&(filt.tf),1,sizeof(filt.tf),arq);
  fwrite(filt.v,filt.tf,sizeof(NOFILT),arq);
  fclose(arq);
}

/*
void FILT::lefilt(char *st)
{ FILE *arq; int e;

  arq=fopen(st,"rb");
  if (arq==NULL) erro("Erro na abertura de arquivo");
  e=setvbuf(arq,NULL,_IOFBF,0x10000);
  if (e!=0) erro("Faltou memoria setvbuf");

  fread(&e,1,sizeof(e),arq); ee.resize(e);
  fread(ee.vet,ee.n(),sizeof(PONTOI2),arq);

  fread(&e,1,sizeof(e),arq); filt.aloc(e); filt.tf=e;
  fread(filt.v,e,sizeof(NOFILT),arq);

  fclose(arq);
}
*/

void impfilt(void)
{ int i;
  printf("****************** impfilt *********************\n");
  for (i=0; i<100; i++) {
    printf("i=%04d e=%04d l=%04d r=%04d\n",
           i,filt.v[i].e,filt.v[i].l,filt.v[i].r);
  }
  printf("\n");
}

BYTE valindee(int i, int iee)
{ return ent(ind.v[i](0)+ee(iee)(0),ind.v[i](1)+ee(iee)(1)); }

BYTE valind(int i)
{ return sai(ind.v[i](0),ind.v[i](1)); }

int mediana(int l, int r, int iee)
// Devolve a primeira posicao m onde aparece o primeiro 1 apos ordenado.
// `A esquerda da posicao devolvida m so ha zeros.
// A partir da posicao m so ha uns.
// Devolve -1 se e' impossivel quebrar (exclusivamente zeros ou uns).
{
  int lo=l; int ro=r;
  // Atras de l so tem zeros
  // Depois de r so tem 255
  while (l<=r) {
    while (valindee(l,iee)==0 && l<=r) l++;
    while (valindee(r,iee)==255 && l<=r) r--;
    if (l<r) {
      PONTOI2 w=ind.v[l]; ind.v[l]=ind.v[r]; ind.v[r]=w; l++; r--;
    }
  }

  #ifdef range
  if (l!=r+1) erro("Erro inesperado 1"); // Depois, joga fora 3 linhas
  if ( !(valindee(l,iee)==255 || ro<l) ) erro("Erro inesperado 2");
  if ( !(valindee(r,iee)==0 || r<lo) ) erro("Erro inesperado 3");
  #endif

  int m;
  if (l==lo || r==ro) m=-1; else m=l;

  return m;
}

//double log2(double x)
//{ return log(x)/M_LN2; }

double elog(double p)
{ double entr;
  if (p<epsilon || 1-epsilon<p) entr=0;
  else entr=-p*log2(p);
  return entr;
}

double entropy(double p)
{ double entr;
  if (p<epsilon || 1-epsilon<p) entr=0;
  else entr=-p*log2(p)-(1-p)*log2(1-p);
  return entr;
}

int pow2(int x)
{ return x*x; }

double condentropy(int l, int r, int iee)
// Obs: A saida e' quantizado em 16 tons de cinza (/16)
// a:
//     sai 0  1  2  3  4 ... 15 som
// ent
//   0     5  1  2  4  0 ...  0  12
//   1     0  8  3  0  0 ...  0  11
// som    [5  9  5  4  0 ...  0] 23
// Trecho entre colchetes nao e' necessario calcular
// Entropia condicional =
//   12/23 * ( elog(5/12) + elog(1/12) + elog(2/12) + elog(4/12) ) +
//   11/23 * ( elog(8/11) + elog(3/11) )
//   = 1.335164
// Obs: elog(p) = -p * log2(p)
{ double entr;
  MATRIZ<int> a(3,17);
  for (int l2=0; l2<2; l2++)
    for (int c2=0; c2<17; c2++)
      a(l2,c2)=0;
  for (int i=l; i<=r; i++) {
    int ient=valindee(i,iee) & 1;
    int isai=valind(i)/16;
    a(ient,isai)++;
  }
  // a(2,0)=a(0,0)+a(1,0); // nao precisa?
  // a(2,1)=a(0,1)+a(1,1); // nao precisa?
  int soma=0;
  for (int c=0; c<16; c++) soma=soma+a(0,c);
  a(0,16)=soma;

  soma=0;
  for (int c=0; c<16; c++) soma=soma+a(1,c);
  a(1,16)=soma;

  a(2,16)=a(0,16)+a(1,16);

  if (a(0,16)==0 || a(1,16)==0) { entr=infinito; }
  else {
    double temp=0.0;
    for (int c=0; c<16; c++) temp=temp+elog(double(a(0,c))/a(0,16));
    entr=(double(a(0,16))/a(2,16))*temp;

    temp=0.0;
    for (int c=0; c<16; c++) temp=temp+elog(double(a(1,c))/a(1,16));
    entr=entr+(double(a(1,16))/a(2,16))*temp;
  }
  return entr;
}

//<<<<<<<<<<<<<<<<<<<<<<<<<< unittree <<<<<<<<<<<<<<<<<<<<<<<<<<<<
int unittree(int l, int r)
{ int s;
  if (modoconf=='m') {
    s=0;
    for (int i=l; i<=r; i++) s=s+valind(i);
    int j=(r-l)/2; // Para acertar arredondamentos
    s=(s+j)/(r-l+1);
  } else if (modoconf=='f') {
    int freq[256];
    for (int i=0; i<256; i++) freq[i]=0;
    for (int i=l; i<=r; i++) freq[valind(i)]++;
    int maxfreq=0; int maxcor=0;
    for (int i=0; i<256; i++) {
      if (freq[i]>maxfreq) { maxfreq=freq[i]; maxcor=i; }
    }
    s=maxcor;
  } else { // (modoconf=='a')
    int freq[256];
    for (int i=0; i<256; i++) freq[i]=0;
    for (int i=l; i<=r; i++) freq[valind(i)]++;
    int minerr=0x7fffffff; int maxcor=0;
    for (int i=0; i<256; i++) { // Testando cor i
      if (freq[i]>0) {
        int err=0;
        for (int j=0; j<256; j++) err=err+abs(i-j)*freq[j];
        if (err<minerr) { minerr=err; maxcor=i; }
      }
    }
    s=maxcor;
  }

  int pf=filt.insere(FIM,s,-1);

  //double desvio=0.0;
  //for (int i=l; i<=r; i++) desvio=desvio+abs(media-valind(i));
  //somadesvio=somadesvio+desvio/(r-l+1);

  folha++;
  return pf;
}

int pwltree(int l, int r, int prof)
{ int pf,plf,prf;

  if (prof<6) fprintf(stderr,"[%02d]",prof);

  bool base;
  if (r-l+1<=nnosfolha) { base=true; } // Numero de nos na folha pequena
  else {
    base=true; BYTE v=valind(l);
    for (int i=l+1; i<=r; i++)
      if (valind(i)!=v) { base=false; break; } // Todas saidas iguais
  }

  if (base) { // Base da recursao
    pf=unittree(l,r);
    return pf;
  }

  // Escolher iee
  double minentropy=infinito;
  int imin=-1;
  for (int i=0; i<ee.n(); i++) {
    double t=condentropy(l,r,i);
    if (t<minentropy) { minentropy=t; imin=i; }
  }
  int iee=imin;

  if (minentropy==infinito) { // Nao conseguiu cortar
    pf=unittree(l,r);
    return pf;
  }

  int m=mediana(l,r,iee);
  #ifdef range
  if (m==-1) erro("Erro inesperado 6");
  #endif
  //filt.v[pf].e=iee;

  plf=pwltree(l,m-1,prof+1);
  if (plf!=-1) prf=pwltree(m,r,prof+1);
  else prf=-1;
  if (plf==-1 || prf==-1) {
    pf=unittree(l,r);
  } else {
    pf=filt.insere(iee,plf,prf);
  }
  return pf;
}

void binariza(IMGGRY& a)
{ for (int l=0; l<a.nl(); l++)
    for (int c=0; c<a.nc(); c++) {
      if (a(l,c)<=127) a(l,c)=0;
      else a(l,c)=255;
    }
}

int main(int argc, char **argv)
{
  if (argc!=6 && argc!=7) {
    printf("BldO: Builds decision-tree for inverse halftoning using Occam's razor\n");
    printf("BldO AX.BMP AY.TGA E.GES FILTER.CTO NNodesLeaf [<<m>>/f/a]\n");
    printf("  AX.BMP = input sample halftone image (uncompressed BMP)\n");
    printf("  AY.TGA = output sample grayscale image (uncompressed TGA)\n");
    printf("  E.GES = window definition (TXT)\n");
    printf("  FILTER.CTO = occam's decision-tree to be generated by BldO (CTO)\n");
    printf("  Operation mode: m:average(default) f:mode a:minimizes-absolute-error\n");
    printf("  NNodesLeaf = Number of nodes per leaf. Must be >= 1.\n");
    printf("The output is quantized in 16 grayscales to compute entropy.\n");
    erro("Error: Invalid quantity of arguments.");
  }
  int t1=centseg();
  le(ent,argv[1]); ent.backg()=255; binariza(ent);
  le(sai,argv[2]); sai.backg()=128;
  leest(argv[3]);

  modoconf='m';
  if (argc==7) {
    modoconf=argv[6][0]; modoconf=tolower(modoconf);
    if (modoconf!='m' && modoconf!='f' && modoconf!='a') modoconf='m';
  }
  //int t2=centseg();

  if (ent.nc()!=sai.nc() || ent.nl()!=sai.nl())
    erro("Error: Two images must have the same dimensions.");
  nc=ent.nc(); nl=ent.nl();

  if (sscanf(argv[5],"%d",&nnosfolha)!=1) erro("NNodesLeaf");

  int temp=1+(2*nc*nl/nnosfolha); // Verificar se precisa de tudo isso
                                  // Somei 1 por precaucao
  if (temp>10000000) temp=10000000;
  filt.aloc(temp);
  ind.aloc(nc*nl);

  for (int l=0; l<nl; l++)
    for (int c=0; c<nc; c++) {
      ind.v[l*nc+c](0)=l; ind.v[l*nc+c](1)=c;
    }

  pwltree(0,ind.ni-1,0);
  //int t3=centseg();

  filt.impfilt(argv[4]);
  int t4=centseg();
  printf("\nDecision-tree construction is finished.\n");
  //printf("-------------------\n");
  //printf("Tamanho da imagem: linhas=%d colunas=%d pixels=%d\n",nl,nc,nl*nc);
  //printf("Numero de nos (int+folhas)=%d\n",filt.tf);
  //printf("Numero de nos alocados    =%d\n",filt.nf);
  //printf("Numero de folhas=%d\n",folha);
  //printf("Media do desvio-medio=%f\n",somadesvio/folha);
  int t=t4-t1; printf("Total processing time (s): %6.2f\n",t/100.0);
  //t=t3-t2; printf("Tempo de processamento: %6.2f\n",t/100.0);
  //t=(t2-t1)+(t4-t3); printf("Tempo de leitura/escrita: %6.2f\n",t/100.0);
}

