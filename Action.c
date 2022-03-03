Action()
{

	long id = 31;
	int  iMethod = 0;
	int  iType = 2;		//1-xml 2-json
	char sAction[1024];
	char sMethod[20];
	char sUrl[128];
	char sJson[1024];
	char sBody[10240];
	char sRSPCOD[7+1];

    //参数区域
	char rspmsg			[10240];
    char sglobalSq      [128+1];
    char sFeePayAcctNo  [128+1];
    char sAccNo         [128+1];

	memset( sAction,0x00,sizeof( sAction ));
	memset( sMethod,0x00,sizeof( sMethod ));
	memset( sUrl,0x00,sizeof( sUrl ));
    memset( sRSPCOD,0x00,sizeof( sRSPCOD ));
    memset( sBody,0x00,sizeof( sBody ));
	memset( rspmsg,0x00,sizeof( rspmsg ));

    memset( sFeePayAcctNo,0x00,sizeof( sFeePayAcctNo ));
    memset( sAccNo,0x00,sizeof( sAccNo ));
    memset( sglobalSq,0x00,sizeof( sglobalSq ));

    //公共域赋值
    iMethod=1;
    #method
    if( iMethod == 1 )
	sprintf( sMethod,"Method=GET" );
    else
	sprintf( sMethod,"Method=POST" );

    //sprintf( sUrl,"%s","URL=http://192.168.118.1:8080" );
    sprintf( sUrl,"%s","URL=http://localhost:8888/api/chain" );

    //参数数据输出测试
    lr_output_message("globalSq:%s",     lr_eval_string("{globalSq}"));
    lr_output_message("FeePayAcctNo:%s", lr_eval_string("{FeePayAcctNo}"));
    lr_output_message("AccNo:%s",        lr_eval_string("{AccNo}"));

    //开始组装参数数据
    //FeePayAcctNo
    sprintf( sglobalSq,"\n<globalSq>%s</globalSq>",lr_eval_string("{globalSq}"));
    sprintf( sFeePayAcctNo,"\n<FeePayAcctNo>%s</FeePayAcctNo>",lr_eval_string("{FeePayAcctNo}"));
    sprintf( sAccNo,"\n<AccNo>%s</AccNo>",lr_eval_string("{AccNo}"));

    //开始组装报文体
    strcat( sBody,"body=<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    strcat( sBody,"\n<Transaction>" );
    strcat( sBody,"\n<msgId>02320230119174735438901</msgId>" );
    strcat( sBody,"\n<msgDate>2023-01-19</msgDate>" );
    //strcat( sBody,"\n<globalSq>00420210811020008263280002000000</globalSq>" );
    strcat( sBody,sglobalSq );
    strcat( sBody,"\n</Transaction>" );
    //报文体组装结束

    //开始事务
    lr_start_transaction("HttpRequest");

    //设置报文头
    web_add_header( "token","asdasdasd" );

    // 默认最大长度为256
    web_set_max_html_param_len("262144");
           
    //自定义截取字符串，根据左右字符串获取数据
    if( iType == 1 ) //Xml
    {
        web_reg_save_param("RSPCOD", "LB=<RSPCOD>", "RB=</RSPCOD>",  LAST);
    	web_reg_save_param("RSPMSG", "LB=<Transaction>", "RB=</Transaction>",  LAST);
    }
    if( iType == 2 ) //Json
    {
        web_reg_save_param("RSPCOD", "LB=\"resCd\":\"", "RB=\",\"resText\"",  LAST);
    	//web_reg_save_param("RSPMSG", "LB=<Transaction>", "RB=</Transaction>",  LAST);
    	web_reg_save_param("RSPMSG", "LB=", "RB=",  LAST);
    }
           
    // REQUEST为内置变量，保存请求的头信息，需在发送URL请求前注册使用，将请求头信息存入字符串RequestHeader中
    web_save_header(REQUEST, "RequestHeader");
           
    // 将RESPONSE保存响应的头信息在字符串ResponseHeader中
    web_save_header(RESPONSE, "ResponseHeader");

    //开始Request请求
    web_custom_request("XmlRequestTest",
        //"URL=http://localhost:8001/payment/create",
        sUrl,
        "Method=POST",
        RecContentType=text/xml",
        "Resource=0",
        "Mode=HTML",
        "EncType=text/xml",
        sBody,
        LAST );

    //字符转码
    lr_convert_string_encoding(lr_eval_string("{RequestHeader}"),LR_ENC_UTF8 ,LR_ENC_SYSTEM_LOCALE,"RequestHeaderUTF8");
    lr_convert_string_encoding(lr_eval_string("{ResponseHeader}"),LR_ENC_UTF8 ,LR_ENC_SYSTEM_LOCALE,"ResponseHeaderUTF8");
    lr_convert_string_encoding(lr_eval_string("{RSPCOD}"),LR_ENC_UTF8 ,LR_ENC_SYSTEM_LOCALE,"RSPCODUTF8");
    lr_convert_string_encoding(lr_eval_string("{RSPMSG}"),LR_ENC_UTF8 ,LR_ENC_SYSTEM_LOCALE,"RSPMSGUTF8");
           
    //输出信息
    lr_output_message("# 请求头信息：\n %s", lr_eval_string("{RequestHeaderUTF8}"));
    lr_output_message("# 响应头信息：\n %s", lr_eval_string("{ResponseHeaderUTF8}"));
    lr_output_message("# 返回码   ：\n %s", lr_eval_string("{RSPCODUTF8}"));
    lr_output_message("# 返回码   ：\n %s", lr_eval_string("{RSPMSGUTF8}"));

    //根据RSPCOD判断是否成功
    strcpy( sRSPCOD,lr_eval_string("{RSPCODUTF8}"));
    strcpy( rspmsg ,lr_eval_string("{RSPMSGUTF8}"));

    if( 0 == strcmp( sRSPCOD,"AAAAAAA"))
    {
        //return 0;
        lr_end_transaction("HttpRequest", LR_PASS);
    }
    else 
    {
        //return -1;
        lr_end_transaction("HttpRequest", LR_FAIL);
    }  

    //return 0;
}
