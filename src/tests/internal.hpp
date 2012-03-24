template <class T> bool 
internaltest (Connector<T>* rc) {
	
	int            i = 0, j = 0, d = 5;
	
	Matrix<cxfl>   cf (d, d, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	Matrix<double> rd (d, d, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	Matrix<short>  si (d, d, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	
	cf.Random(); 
	rd.Random();
	si.Random();

	Matrix<size_t> m (3,2);
	m (0,0) = 1;
	m (0,1) = 3;
	m (1,0) = 1;
	m (1,1) = 4;
	m (2,0) = 1;
	m (2,1) = 5;

	std::cout << m << std::endl;
	
	Matrix<size_t> mg = Matrix<size_t>::MeshGrid(m);
#ifdef HAVE_MAT_H	
	//mg.MXDump("mg.mat", "mg");
#endif
	
	std::cout << cf << std::endl;
	std::cout << rd << std::endl;
	std::cout << si << std::endl;
	
	// Test casting
	Matrix<cxdb> cd = (Matrix<cxdb>) cf;

	rc->ReadConfig ("test.xml");

	rc->Init (test);

	rc->SetMatrix ("cf", cf);
	rc->SetMatrix ("si", si);
	rc->SetMatrix ("rd", rd);
	
	time_t seconds = time (NULL);
	char   uid[16];
	sprintf(uid,"%ld",seconds);
	
	rc->SetAttribute ("UID", uid);
	rc->SetAttribute ("Pi", 3.14156);
	rc->SetAttribute ("Dim", d);
	
	rc->Process (test);
	
	rc->GetMatrix ("cf", cf);
	rc->GetMatrix ("rd", rd);
	rc->GetMatrix ("si", si);

	rc->Finalise (test);

	cout << "We're good" << endl;

	return true;
	
}
