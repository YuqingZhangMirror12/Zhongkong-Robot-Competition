#pragma once
#include<Python.h>
#include<cstdio>

PyObject *pModule, *pDict, *pFunc, *pArgs;
int Judge(int num);
int Init_py()
{
	Py_Initialize();
	if (!Py_IsInitialized()) {
		return -1;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("print('---import sys---')");
	PyRun_SimpleString("sys.path.append('D:\\CV_Py')");

	pModule = PyImport_ImportModule("test1");
	if (!pModule) {
		printf("can't find test1.py");
		getchar();
		return -1;
	}
	printf("good!\n");
	pFunc = PyObject_GetAttrString(pModule, "predic");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		printf("can't find function [predic]");
		getchar();
		return -1;
	}
	printf("excellen!\n");
	Judge(0);
}
int Judge(int num)
{
	pArgs = PyTuple_New(1);//函数调用的参数传递均是以元组的形式打包的,2表示参数个数
	PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", num));//0--序号,i表示创建int型变量

	PyObject *pReturn = NULL;
	pReturn = PyEval_CallObject(pFunc, pArgs);//调用函数
												//将返回值转换为int类型
	int result;
	PyArg_Parse(pReturn, "i", &result);//i表示转换成int型变量
	printf("%d\n", result);
	return result;
}
