#include <iostream>
#include "WebSocket.h"

#define SEND_DATA \
"{ \
\"key\" : \"c36faed1-a874-4f42-a6eb-2df9ba3b156b\", \
\"orderType\" : \"PRINT\", \
\"parameters\" : { \
	\"printData\" : [ \
		\"/Z9UqyNHS9fRUt44WYxOZwCTnc7xxNvBD+FOxzOAiWU9tFWbOAkVD3Nk+UzoEhyMVFaSquuEUrVxC85yogpSP5VVWU4URiXB9R9MFPuRAPTv/WD9AEXRYoymru9kfC2qPc/qyFTmuxhHBonLnEZEwOT87qcFsCSv7kpGOANPfkJlv3i5FAsbaAKG2qVGtl9H2RuCZYkfg3k4L70KmdPb49srg2p61t0DoU031tDh56fh2BdfWU1N1wrtqE0V0/8Ckh8bbrDkN727aG/tBurkKNWSIxR9QwI5+b8Oj2KQ5DOCx46vUwkDgvnGNfado4crsw0PSwl+r9ely74wc7m5E7K9Ww7AoEtKDfTJd3HTxxhIzMc2rryAOxfBEh2xaRNgJw03trwkpfRxEmnJaezUpjfndRjdzAH9VEdzjmSfT3EAhXlHY/5eoq7xl4w87WFk/UXNj3avXDTe0F0ma2mQCGZGADT7yOifVANZKvJWzKB/HgC82PcVCfT1KH+yL4QG1bMSkNEIiO6WV8uOPlb64i3AkUu5Ten/WDtDNEeLJy6UW8taSzFJBS1f47/lCIy2wsBtYW5NYr9R2KYyM4iomkyGjbPdNT2zYcU4+ClxzO4KAvCFo6z27tnRMsXbrTBVq+Q+C5Bt1sbAD5IMwf77ex0jkVAYyVtN8nrOLELFjvOwnDMB0FLiATkLH254jxylUwevVXb2S9zq5yJd/Zwinaq1h+rbJCIbmwj4Z7Lx0wyWLQfJnk53AE6LOw2c6L5sMotWZKDJ877H14dT3LetyKa5h2gWDwqqMSLDnn+15P+Z6vd+5P6vlUJ552CC9GYvXwLp4EcdPTIXc71kyXtxZyHLUVNbD2O3HA+f7GWK60ARU9qccSTystwxGSTIetD6iwDKEZla9qFMx7BDXT7g9PBshyMn6odAkNB6OTmLPqf8UakRfFQWMeI8sqmReg5m7fUuzwaLJTbHkHUKyNMIFMNbYNBHh6yLibl7RytKA2HaNuIqoq/N46pQspG/Mc2l4mew/GgST+9vlC0tSq8A1RVrc5IrJXA205eJiHuhQM6hHkkO02VtOtVOyT7TbKXVh3ZkhzNm3lEON/2F8csha/nmSLuDofO1Okw8q4U6H5eu95tkMezAsfvkv+BNivkltgthwYhKi8FV5JCWIrxMjMRTh942O24KbrGc6blEoXUhw3C/yAJrG5LUDrktIjrYRePwhvp6Qx2LPoc9MfSedQ==\" \
	], \
		\"printName\" : \"Microsoft Print to PDF\", \
			\"tempUrl\" : \"https://storage.360buyimg.com/jdl-template/templates-3f3ffc68-0045-496d-a25e-ac123cae5fbb.1631760624107.txt\" \
} \
}"

int main()
{
	{
		WebSocket websocket;

		if (websocket.Start("ws://127.0.0.1:9113"))
		{
			std::string strInput;
			while (getline(std::cin, strInput) && strInput != "Q")
			{
				bool bRet = websocket.SendTextData(SEND_DATA);

				std::string strRecv = websocket.Wait(10);

				std::cout << (strRecv.empty() ? "NULL" : strRecv) << std::endl;

			}
		}
	}

	//websocket.Stop();

	getchar();

}
