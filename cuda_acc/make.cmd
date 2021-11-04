@REM "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\bin\nvcc.exe" --shared imgalg_cuda.cu -o imgalg_cuda.dll 
@REM g++ .\test.cpp -l -L. -limgalg_cuda -L"C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.0/lib/x64/" -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\include" -o test.exe -g

"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.0\bin\nvcc.exe" .\ImgAlgCuda.cu -o test.exe
nvprof test.exe