#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argvc, char** argv){
	const string source = argv[1];
	int cont = 0, taxa = 2;
	
	// Abertura do vídeo e obtenção de alguns dos seus parâmetros
	VideoCapture inputVideo(source);
	if (!inputVideo.isOpened())
    	{
        	cout  << "O vídeo não pode ser aberto: " << source << endl;
        	return -1;
    	}
    	
    	Size S = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH),    
                      (int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));
        
     	// Criação do arquivo de vídeo para escrita
    	VideoWriter outputVideo;
    	outputVideo.open("saida.mkv", CV_FOURCC('D', 'I', 'V', '3'), inputVideo.get(CAP_PROP_FPS)/taxa, S, true);	    
	if (!outputVideo.isOpened())
	{
		cout  << "Não é possivel abrir o arquivo para escrita: " << source << endl;
		return -1;
	}
	
	// Parâmetros para efeito Tiltshift
	Mat mask(9,9,CV_32F), mask1;
	Mat image32f, imageFiltered, result;
	Mat image1, image2, res;

	double alfa;

	double altura = 0.3 * S.height; // A altura da região central será 30% da altura do vídeo 
	double centro = 0.5 * S.height; // A posição vertical do centro da região que entrará em foco estará bem no meio do vídeo
	double decaimento = 10; // O decaimento da região borrada será 10
	double l1 = centro - altura/2;
	double l2 = centro + altura/2;
	
	float media[] = {1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
 	 	         1,1,1,1,1,1,1,1,1,
		         1,1,1,1,1,1,1,1,1};
	
  	mask = Mat(9, 9, CV_32F, media); 
	scaleAdd(mask, 1/81.0, Mat::zeros(9,9,CV_32F), mask1);
        swap(mask, mask1);
		
	for(;;)
   	{
   	   	cont++;
   		
   		if(cont > 100)
   		{
			inputVideo >> image1;              
			if (image1.empty()) break; // checa se o vídeo acabou
			
			image2 = image1.clone();
			image2.convertTo(image32f, CV_32F);
			
			// A imagem é borrada várias vezes para poder ser notado melhor o efeito de borramento
			for (int i = 0; i < 100; i++) 
				filter2D(image32f, imageFiltered, image32f.depth(), mask, Point(2,2), 0);
			
			// A variável 'result' guarda a imagem borrada
			imageFiltered.convertTo(result, CV_8U); 
			
			for(int i = 0; i < result.size().height; i++)
			{
				alfa = 0.5 * (tanh((i - l1) / decaimento) - tanh((i - l2) / decaimento));
				addWeighted(image1.row(i), alfa, result.row(i), 1.0 - alfa, 0.0, image2.row(i));
			}
       		
       			outputVideo << image2;
       			cont = 0;
       		}
    	}

	return 0;
}	
