#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <time.h>
#include <sys/types.h>
#include "../libs/diskimg/DiskImg.h"
#include "../libs/diskimg/DiskImgDetail.h"
#include "../libs/nufxlib/NufxLib.h"

using namespace DiskImgLib;

#pragma pack(push, 2)
struct AFP_Info {
    uint32_t magic;
    uint32_t version;
    uint32_t file_id;
    uint32_t backup_date;
    uint8_t finder_info[32];
    uint16_t prodos_file_type;
    uint32_t prodos_aux_type;
    uint8_t reserved[6];
};
#pragma pack(pop)

int usage()
{
    printf("\nGolden Gate -> Image File (v1.0)\n");
    printf("Inserts a file built with Golden Gate into a disk image file (.po), preserving resource information\n");
    printf("https://github.com/BrianPeek/gg2img\n");
    printf("\nUsage: gg2img [IIgs file] [Image file]\n\n");
    return -1;
}

void DebugMsgHandler(const char* file, int line, const char* msg)
{
    printf("%s:%d - %s\n", file, line, msg);
}

NuResult NufxErrorMsgHandler(NuArchive* pArchive, void* vErrorMessage)
{
    const NuErrorMessage* pErrorMessage = (const NuErrorMessage*)vErrorMessage;

    printf("NuLib: %s:%d - %s\n", pErrorMessage->file, pErrorMessage->line, pErrorMessage->message);

    return kNuOK;
}

void readfile(const char* filename, const char* resource, void** data, long* fileSize)
{
    FILE* fin = NULL;
    char fullname[_MAX_PATH];

    if(resource != NULL)
        sprintf_s(fullname, "%s:%s", filename, resource);
    else
        sprintf_s(fullname, "%s", filename);

    fopen_s(&fin, fullname, "rb");
    if (fin != NULL)
    {
        fseek(fin, 0, SEEK_END);
        long size = ftell(fin);

        if(fileSize != NULL)
            *fileSize = size;

        fseek(fin, 0, SEEK_SET);
        *data = (unsigned char*)malloc(size);
        if (*data != NULL)
            fread(*data, 1, size, fin);
        fclose(fin);
    }
}

void writefile(const char* imgFileName, const char* destFileName, const unsigned char* fileData, long fileDataSize, const unsigned char* rsrcData, long rsrcDataSize, AFP_Info* afpInfo)
{
    DiskImgLib::DiskImg diskImg;
    DiskImgLib::DIError dierr;

    dierr = diskImg.OpenImage(imgFileName, '\\', false);
    dierr = diskImg.AnalyzeImage();
    auto format = diskImg.GetFSFormat();
    auto diskFS = diskImg.OpenAppropriateDiskFS();
    diskFS->SetScanForSubVolumes(DiskFS::kScanSubEnabled);
    dierr = diskFS->Initialize(&diskImg, DiskFS::kInitFull);

    DiskFS::CreateParms parms;
    parms.fileType = afpInfo->prodos_file_type;
    parms.auxType  = afpInfo->prodos_aux_type;
    parms.pathName = destFileName;
    parms.access   = A2FileProDOS::kAccessRead | A2FileProDOS::kAccessWrite | A2FileProDOS::kAccessBackup | A2FileProDOS::kAccessRename | A2FileProDOS::kAccessDelete;
    parms.createWhen  = time(0);
    parms.modWhen     = time(0);
    parms.storageType = A2FileProDOS::kStorageExtended;

    A2File* a2File = NULL;
    a2File = diskFS->GetFileByName(destFileName);
    if(a2File != NULL)
        dierr = diskFS->DeleteFileW(a2File);

    dierr = diskFS->CreateFile(&parms, &a2File);

    A2FileDescr* a2FileDescr;
    dierr = a2File->Open(&a2FileDescr, false, false);
    dierr = a2FileDescr->Write(fileData, fileDataSize);
    dierr = a2FileDescr->Close();

    dierr = a2File->Open(&a2FileDescr, false, true);
    dierr = a2FileDescr->Write(rsrcData, rsrcDataSize);
    dierr = a2FileDescr->Close();

    delete diskFS;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
        return usage();

    char* inputFile = argv[1];
    char* imgFile   = argv[2];

    AFP_Info* afpInfo = NULL;
    unsigned char* afpResource = NULL;
    unsigned char* fileData = NULL;

    long fileDataSize = 0;
    long fileRsrcSize = 0;

    readfile(argv[1], "AFP_AfpInfo", (void**)&afpInfo, NULL);
    readfile(argv[1], "AFP_Resource", (void**)&afpResource, &fileRsrcSize);
    readfile(argv[1], NULL, (void**)&fileData, &fileDataSize);

    DiskImgLib::Global::SetDebugMsgHandler(DebugMsgHandler);
    DiskImgLib::Global::AppInit();

    NuSetGlobalErrorMessageHandler(NufxErrorMsgHandler);

    char fileName[_MAX_FNAME];
    errno_t err = _splitpath_s(argv[1], NULL, 0, NULL, 0, fileName, _MAX_FNAME, NULL, 0);

    writefile(argv[2], fileName, fileData, fileDataSize, afpResource, fileRsrcSize, afpInfo);

    DiskImgLib::Global::AppCleanup();

    if(afpInfo != NULL)
        free(afpInfo);

    if(afpResource != NULL)
        free(afpResource);

    if(fileData != NULL)
        free(fileData);
}

