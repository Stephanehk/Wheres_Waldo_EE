#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <imgall>

int main(int argc, char **argv)
{ IMGGRY ide,pro; int l,c,np; double da,dq,qide,dqn;
  BYTE maxide; int nd,d,i; char modo;

  if (argc!=3 && argc!=4 && argc!=5) {
    printf("DistG: Computes the difference between two grayscale images.\n");
    printf("DistG IDEAL.TGA PROCESSED.TGA [m / <<s>> / c] [n]\n");
    printf("  IDEAL.TGA and PROCESSED.TGA = images to be compared\n");
    printf("  Verbose: m=minimal s=standard(default) c=complete\n");
    printf("  n=number of external columns/rows to be ignored\n");
    erro("Error: invalid number of parameters");
  }
  le(ide,argv[1]); le(pro,argv[2]);
  if (ide.nl()!=pro.nl() || ide.nc()!=pro.nc())
    erro("Two images have different dimensions.\n");

  modo='s';
  if (argc==4 || argc==5) {
    modo=tolower(argv[3][0]);
    if (modo!='m' && modo!='s' && modo!='c')
    erro("Verbose mode must be M or S or C");
  }

  int n=0;
  if (argc==5) {
    if (sscanf(argv[4],"%d",&n)!=1) erro("Error in reading n");
    if (n<0) erro("Error: n<0");
  }

  da=0.0; dq=0.0; qide=0.0; maxide=0; nd=0; np=(ide.nl()-2*n)*(ide.nc()-2*n);
  dqn=0.0;

  for (l=0+n; l<pro.nl()-n; l++)
    for (c=0+n; c<pro.nc()-n; c++) {
      d=ide(l,c)-pro(l,c);
      if (d!=0) {
        nd++; da=da+abs(d); dq=dq+d*d;
        double temp=G2F(ide(l,c))-G2F(pro(l,c));
        dqn=dqn+temp*temp;
      }

      qide=qide+ide(l,c)*ide(l,c);
      if (ide(l,c)>maxide) maxide=ide(l,c);
    }

  double media=da/np; double desvio=0.0;
  for (l=0+n; l<pro.nl()-n; l++)
    for (c=0+n; c<pro.nc()-n; c++) {
      double diferenca=ide(l,c)-pro(l,c);
      desvio=desvio+(media-diferenca)*(media-diferenca);
    }
  desvio=sqrt(desvio/np);

 for (i=0; i<argc; i++) printf("%s ",argv[i]);
 printf("\n");
 if (modo=='c') {
  printf("Rows=%d Columns=%d TotalPixels=%d\n",ide.nl(),ide.nc(),ide.nl()*ide.nc());
  printf("Rows-2n=%d Columns-2n=%d UsedPixels=%d\n",ide.nl()-2*n,ide.nc()-2*n,np);
  printf("Number of different pixels.......: %20d\n",nd);
  printf("%% of different pixels...........: %19.2f%%\n",(100.0*nd)/np);
  printf("--------------------\n");
  printf("Sum of absolute differences......: %20.0f\n",da);
  printf("MAE (max=255)....................: %20.2f\n",da/np);
  printf("  MAE = mean absolute error\n");
  printf("Standard-deviation (max=255).....: %20.4f\n",desvio);
  printf("MAE (max=100%%)...................: %19.2f%%\n",(100.0/255)*(da/np));
  printf("Standard-deviation (max=100%%)....: %19.4f%%\n",(100.0/255)*desvio);
  printf("--------------------\n");
  printf("Sum of squared differences.......: %20.0f\n",dq);
  printf("MSE (max=255)....................: %20.2f\n",sqrt(dq/np));
  printf("  MSE = mean square error\n");
  printf("MSE (max=100%%)...................: %19.2f%%\n",(100.0/255)*sqrt(dq/np));
  printf("--------------------\n");
  printf("Sum of normalized square differen: %20.0f\n",dqn);
  printf("Average normalized square differe: %20.4f\n",dqn/np);
  printf("NMSE (max=1.0)...................: %20.4f\n",sqrt(dqn/np));
  printf("  NMSE = normalized mean square error\n");
  printf("NMSE (max=100%%)..................: %19.2f%%\n",(100.0)*sqrt(dqn/np));
  printf("--------------------\n");
  printf("E_NLSE (norm. least-squares err).: %20f\n",dq/qide);
  printf("SNR (signal-to-noise ratio)......: %20.4f dB\n",-10*log10(dq/qide));
  printf("E_PLSE (peak least-squares error): %20f\n",
         dq/(double(np)*maxide*maxide));
  printf("PSNR (peak signal-to-noise ratio): %20.4f dB\n",
         -10*log10(dq/(double(np)*maxide*maxide))
        );
  printf("Highest value in ideal image.....: %20d\n",maxide);
  printf("PSNR considering highest=255.....: %20.4f dB\n",
         -10*log10(dq/(double(np)*255*255))
        );
 } else if (modo=='s') {
  printf("%% of different pixels...........: %19.2f%%\n",(100.0*nd)/np);
  printf("MAE (max=100%%)...................: %19.2f%%\n",(100.0/255)*(da/np));
  printf("Standard-deviation (max=100%%)....: %19.4f%%\n",(100.0/255)*desvio);
  printf("MSE (max=100%%)...................: %19.2f%%\n",(100.0/255)*sqrt(dq/np));
  printf("SNR (signal-to-noise ratio)......: %20.4f dB\n",-10*log10(dq/qide));
  printf("PSNR considering highest=255.....: %20.4f dB\n",
         -10*log10(dq/(double(np)*255*255))
        );
 } else {
  printf("MAE (max=100%%)...................: %19.2f%%\n",(100.0/255)*(da/np));
  printf("PSNR considering highest=255.....: %20.4f dB\n",
         -10*log10(dq/(double(np)*255*255))
        );
 }
 printf("\n");
}

