template <class T> bool
fftwtest (Connector<T>* rc) {

	std::string in  = std::string (base + std::string ("/infft.h5"));
	std::string out = std::string (base + std::string ("/outfft.h5"));
	
	Matrix<cxfl> m;

	m.Read (in, "img");
	m = FFT::Forward(m);
	m = FFT::Backward(m);
	m.Dump (out, "img");

	return true;

}
