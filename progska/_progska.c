
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "progska.h"

static char module_docstring[] =
    "This module provides a fast uploading interface for SKARABs.";
static char upload_docstring[] =
    "Upload the given bin file to the given list of SKARAB boards.";

static PyObject *casperfpga_progskaupload(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
    {"upload", casperfpga_progskaupload, METH_VARARGS, upload_docstring},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef progska =
{
    PyModuleDef_HEAD_INIT,
    "progska", //name of module
    "", //Module docstring
    -1, //size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
    module_methods
};

PyMODINIT_FUNC PyInit_progska(void) {
    return PyModule_Create(&progska);
}

static PyObject *casperfpga_progskaupload(PyObject *self, PyObject *args) {
    /*
    Process the arguments from Python and then pass them to the progska
    C method.
    */
    const char *binfile, *skarabname;
    PyObject *hostlist_obj, *host_list, *item;
    const char *packet_size;
    int host_ctr, num_hosts;
    int verbose;
    verbose = 0;
    // parse the input tuple
    if (!PyArg_ParseTuple(args, "sOs", &binfile, &hostlist_obj, &packet_size))
        return NULL;

    if(strlen(binfile) <= 0){
        PyErr_SetString(PyExc_RuntimeError,
            "Must provide a bin file to upload.");
        return NULL;
    }
    if(verbose > 0)
        printf("Programming: %s\n", binfile);

        host_list = PySequence_Fast(hostlist_obj, "Expected a list of hosts.");
        if (host_list == NULL) {
            return NULL;
        }
        num_hosts = (int)PySequence_Fast_GET_SIZE(host_list);
        if(num_hosts <= 0){
            Py_DECREF(host_list);
            PyErr_SetString(PyExc_RuntimeError,
                "Must provide at least one host to which to upload the bin file.");
            return NULL;
        }
        if(verbose > 0)
            printf("Given %i hosts.\n", num_hosts);

        char **mainargs = NULL;
        int num_mainargs = num_hosts + 5;
        mainargs = (char**)malloc((size_t)num_mainargs * sizeof(char*));
        if (mainargs == NULL) {
            Py_DECREF(host_list);
            PyErr_NoMemory();
            return NULL;
        }

        const char *progska = "progska";
        const char *dashs = "-s";
        const char *dashf = "-f";
        mainargs[0] = (char*)progska;
        mainargs[1] = (char*)dashs;
        mainargs[2] = (char*)packet_size;
        mainargs[3] = (char*)dashf;
        mainargs[4] = (char*)binfile;
        for (host_ctr = 0; host_ctr < num_hosts; host_ctr++) {
            item = PySequence_Fast_GET_ITEM(host_list, host_ctr);
            const char *hostname = NULL;
            if (PyUnicode_Check(item)) {
                hostname = PyUnicode_AsUTF8(item);
            } else if (PyBytes_Check(item)) {
                hostname = PyBytes_AsString(item);
            } else {
                free(mainargs);
                Py_DECREF(host_list);
                PyErr_SetString(PyExc_TypeError, "Host entries must be str or bytes.");
                return NULL;
            }
            if (hostname == NULL) {
                free(mainargs);
                Py_DECREF(host_list);
                return NULL; // Conversion failed; Python error already set.
            }
            mainargs[host_ctr + 5] = (char*)hostname;
            if(verbose > 0)
                printf("\t%s\n", hostname);
        }
// call Marc's main function to do the upload
    int marcresult;
    marcresult = main(num_mainargs, mainargs);
    PyObject *ret = Py_BuildValue("i", marcresult);
    free(mainargs);
    Py_DECREF(host_list);
    return ret;
}

// end
