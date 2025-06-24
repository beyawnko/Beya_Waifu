// realesrgan implemented with ncnn library
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <queue>
#include <vector>
#include <clocale>
#include <filesystem>
namespace fs = std::filesystem;


#if _WIN32
// image decoder and encoder with wic
#include "wic_image.h"
#else // _WIN32
// image decoder and encoder with stb
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_STDIO
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif // _WIN32
#include "webp_image.h"

#if _WIN32
#include <wchar.h>
static wchar_t* optarg = NULL;
static int optind = 1;
static wchar_t getopt(int argc, wchar_t* const argv[], const wchar_t* optstring)
{
    if (optind >= argc || argv[optind][0] != L'-')
        return -1;

    wchar_t opt = argv[optind][1];
    const wchar_t* p = wcschr(optstring, opt);
    if (p == NULL)
        return L'?';

    optarg = NULL;

    if (p[1] == L':')
    {
        optind++;
        if (optind >= argc)
            return L'?';

        optarg = argv[optind];
    }

    optind++;

    return opt;
}

static std::vector<int> parse_optarg_int_array(const wchar_t* optarg)
{
    std::vector<int> array;
    array.push_back(_wtoi(optarg));

    const wchar_t* p = wcschr(optarg, L',');
    while (p)
    {
        p++;
        array.push_back(_wtoi(p));
        p = wcschr(p, L',');
    }

    return array;
}
#else // _WIN32
#include <unistd.h> // getopt()

static std::vector<int> parse_optarg_int_array(const char* optarg)
{
    std::vector<int> array;
    array.push_back(atoi(optarg));

    const char* p = strchr(optarg, ',');
    while (p)
    {
        p++;
        array.push_back(atoi(p));
        p = strchr(p, ',');
    }

    return array;
}
#endif // _WIN32

// ncnn
#include "cpu.h"
#include "gpu.h"
#include "platform.h"

#include "realesrgan.h"

#include "filesystem_utils.h"

static int queue_size = 8;

static void print_usage()
{
    fprintf(stderr, "Usage: realesrgan-ncnn-vulkan -i infile -o outfile [options]...\n\n");
    fprintf(stderr, "  -h                   show this help\n");
    fprintf(stderr, "  -i input-path        input image path (jpg/png/webp) or directory\n");
    fprintf(stderr, "  -o output-path       output image path (jpg/png/webp) or directory\n");
    fprintf(stderr, "  -s scale             upscale ratio (can be 2, 3, 4. default=4)\n");
    fprintf(stderr, "  -t tile-size         tile size (>=32/0=auto, default=0) can be 0,0,0 for multi-gpu\n");
    fprintf(stderr, "  -m model-path        folder path to the pre-trained models. default=models\n");
    fprintf(stderr, "  -n model-name        model name (default=realesr-animevideov3, can be realesr-animevideov3 | realesrgan-x4plus | realesrgan-x4plus-anime | realesrnet-x4plus)\n");
    fprintf(stderr, "  -g gpu-id            gpu device to use (default=auto) can be 0,1,2 for multi-gpu\n");
    fprintf(stderr, "  -j load:proc:save    thread count for load/proc/save (default=1:2:2) can be 1:2,2,2:2 for multi-gpu\n");
    fprintf(stderr, "  -x                   enable tta mode\n");
    fprintf(stderr, "  -f format            output image format (jpg/png/webp, default=ext/png)\n");
    fprintf(stderr, "  -v                   verbose output\n");
    fprintf(stderr, "  -q queue-size        task queue size (default=8)\n");
}

class Task
{
public:
    int id;
    int webp;

    path_t inpath;
    path_t outpath;

    ncnn::Mat inimage;
    ncnn::Mat outimage;
};

class TaskQueue
{
public:
    TaskQueue()
    {
    }

    void put(const Task& v)
    {
        lock.lock();

        while (tasks.size() >= queue_size)
        {
            condition.wait(lock);
        }

        tasks.push(v);

        lock.unlock();

        condition.signal();
    }

    void get(Task& v)
    {
        lock.lock();

        while (tasks.size() == 0)
        {
            condition.wait(lock);
        }

        v = tasks.front();
        tasks.pop();

        lock.unlock();

        condition.signal();
    }

private:
    ncnn::Mutex lock;
    ncnn::ConditionVariable condition;
    std::queue<Task> tasks;
};

TaskQueue toproc;
TaskQueue tosave;

class LoadThreadParams
{
public:
    int scale;
    int jobs_load;

    // session data
    std::vector<path_t> input_files;
    std::vector<path_t> output_files;
};

void* load(void* args)
{
    const LoadThreadParams* ltp = (const LoadThreadParams*)args;
    const int count = ltp->input_files.size();
    const int scale = ltp->scale;

    #pragma omp parallel for schedule(static,1) num_threads(ltp->jobs_load)
    for (int i=0; i<count; i++)
    {
        const path_t& imagepath = ltp->input_files[i];

        int webp = 0;

        unsigned char* pixeldata = 0;
        int w;
        int h;
        int c;

#if _WIN32
        FILE* fp = _wfopen(imagepath.c_str(), L"rb");
#else
        FILE* fp = fopen(imagepath.c_str(), "rb");
#endif
        if (fp)
        {
            // read whole file
            unsigned char* filedata = 0;
            int length = 0;
            {
                fseek(fp, 0, SEEK_END);
                length = ftell(fp);
                rewind(fp);
                filedata = (unsigned char*)malloc(length);
                if (filedata)
                {
                    fread(filedata, 1, length, fp);
                }
                fclose(fp);
            }

            if (filedata)
            {
                pixeldata = webp_load(filedata, length, &w, &h, &c);
                if (pixeldata)
                {
                    webp = 1;
                }
                else
                {
                    // not webp, try jpg png etc.
#if _WIN32
                    pixeldata = wic_decode_image(imagepath.c_str(), &w, &h, &c);
#else // _WIN32
                    pixeldata = stbi_load_from_memory(filedata, length, &w, &h, &c, 0);
                    if (pixeldata)
                    {
                        // stb_image auto channel
                        if (c == 1)
                        {
                            // grayscale -> rgb
                            stbi_image_free(pixeldata);
                            pixeldata = stbi_load_from_memory(filedata, length, &w, &h, &c, 3);
                            c = 3;
                        }
                        else if (c == 2)
                        {
                            // grayscale + alpha -> rgba
                            stbi_image_free(pixeldata);
                            pixeldata = stbi_load_from_memory(filedata, length, &w, &h, &c, 4);
                            c = 4;
                        }
                    }
#endif // _WIN32
                }

                free(filedata);
            }
        }
        if (pixeldata)
        {
            Task v;
            v.id = i;
            v.inpath = imagepath;
            v.outpath = ltp->output_files[i];

            v.inimage = ncnn::Mat(w, h, (void*)pixeldata, (size_t)c, c);
            v.outimage = ncnn::Mat(w * scale, h * scale, (size_t)c, c);

            path_t ext = get_file_extension(v.outpath);
            if (c == 4 && (ext == PATHSTR("jpg") || ext == PATHSTR("JPG") || ext == PATHSTR("jpeg") || ext == PATHSTR("JPEG")))
            {
                path_t output_filename2 = ltp->output_files[i] + PATHSTR(".png");
                v.outpath = output_filename2;
#if _WIN32
                fwprintf(stderr, L"image %ls has alpha channel ! %ls will output %ls\n", imagepath.c_str(), imagepath.c_str(), output_filename2.c_str());
#else // _WIN32
                fprintf(stderr, "image %s has alpha channel ! %s will output %s\n", imagepath.c_str(), imagepath.c_str(), output_filename2.c_str());
#endif // _WIN32
            }

            toproc.put(v);
        }
        else
        {
#if _WIN32
            fwprintf(stderr, L"decode image %ls failed\n", imagepath.c_str());
#else // _WIN32
            fprintf(stderr, "decode image %s failed\n", imagepath.c_str());
#endif // _WIN32
        }
    }

    return 0;
}

// Helper function to set stdio to binary mode
#if _WIN32
#include <fcntl.h>
#include <io.h>
#endif
void set_binary_stdio() {
#if _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#else
    // freopen(NULL, "rb", stdin);
    // freopen(NULL, "wb", stdout);
#endif
}


#if _WIN32
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    path_t inputpath;
    path_t outputpath;
    int scale = 4;
    std::vector<int> tilesize;
    path_t model = PATHSTR("models");
    path_t modelname = PATHSTR("realesr-animevideov3");
    std::vector<int> gpuid;
    int jobs_load = 1;
    std::vector<int> jobs_proc;
    int jobs_save = 2;
    int verbose = 0;
    int tta_mode = 0;
    path_t format = PATHSTR("png");

    // New flags for pipe mode
    bool use_pipe_mode = false;
    int input_width = 0;
    int input_height = 0;
    int input_channels = 0;
    std::string pixel_format_str = "BGR"; // Default

#if _WIN32
    setlocale(LC_ALL, "");
    // Manual parsing for long options first
    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"--use-pipe") == 0) {
            use_pipe_mode = true;
        } else if (wcscmp(argv[i], L"--input-width") == 0 && i + 1 < argc) {
            input_width = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--input-height") == 0 && i + 1 < argc) {
            input_height = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--input-channels") == 0 && i + 1 < argc) {
            input_channels = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--pixel-format") == 0 && i + 1 < argc) {
            wchar_t* format_wstr = argv[++i];
            char format_cstr[256];
            size_t num_converted;
            wcstombs_s(&num_converted, format_cstr, sizeof(format_cstr), format_wstr, _TRUNCATE);
            if (num_converted > 0) {
                pixel_format_str = format_cstr;
            }
        }
    }
    wchar_t opt;
    while ((opt = getopt(argc, argv, L"i:o:s:t:m:n:g:j:f:q:vxh")) != (wchar_t)-1)
    {
        switch (opt)
        {
        case L'i':
            inputpath = optarg;
            break;
        case L'o':
            outputpath = optarg;
            break;
        case L's':
            scale = _wtoi(optarg);
            break;
        case L't':
            tilesize = parse_optarg_int_array(optarg);
            break;
        case L'm':
            model = optarg;
            break;
        case L'n':
            modelname = optarg;
            break;
        case L'g':
            gpuid = parse_optarg_int_array(optarg);
            break;
        case L'j':
            swscanf(optarg, L"%d:%*[^:]:%d", &jobs_load, &jobs_save);
            jobs_proc = parse_optarg_int_array(wcschr(optarg, L':') + 1);
            break;
        case L'f':
            format = optarg;
            break;
        case L'q':
            queue_size = _wtoi(optarg);
            break;
        case L'v':
            verbose = 1;
            break;
        case L'x':
            tta_mode = 1;
            break;
        case L'h':
        default:
            print_usage();
            return -1;
        }
    }
#else // _WIN32
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--use-pipe") == 0) {
            use_pipe_mode = true;
        } else if (strcmp(argv[i], "--input-width") == 0 && i + 1 < argc) {
            input_width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--input-height") == 0 && i + 1 < argc) {
            input_height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--input-channels") == 0 && i + 1 < argc) {
            input_channels = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--pixel-format") == 0 && i + 1 < argc) {
            pixel_format_str = argv[++i];
        }
    }
    int opt;
    while ((opt = getopt(argc, argv, "i:o:s:t:m:n:g:j:f:q:vxh")) != -1)
    {
        switch (opt)
        {
        case 'i':
            inputpath = optarg;
            break;
        case 'o':
            outputpath = optarg;
            break;
        case 's':
            scale = atoi(optarg);
            break;
        case 't':
            tilesize = parse_optarg_int_array(optarg);
            break;
        case 'm':
            model = optarg;
            break;
        case 'n':
            modelname = optarg;
            break;
        case 'g':
            gpuid = parse_optarg_int_array(optarg);
            break;
        case 'j':
            sscanf(optarg, "%d:%*[^:]:%d", &jobs_load, &jobs_save);
            jobs_proc = parse_optarg_int_array(strchr(optarg, ':') + 1);
            break;
        case 'f':
            format = optarg;
            break;
        case 'q':
            queue_size = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'x':
            tta_mode = 1;
            break;
        case 'h':
        default:
            print_usage();
            return -1;
        }
    }
#endif // _WIN32

    if (use_pipe_mode) {
        if (input_width <= 0 || input_height <= 0 || (input_channels != 3 && input_channels != 4)) {
            fprintf(stderr, "Error: For --use-pipe mode, --input-width, --input-height, and --input-channels (3 or 4) must be specified and valid.\n");
            print_usage();
            return -1;
        }
        if (pixel_format_str != "RGB" && pixel_format_str != "BGR" && pixel_format_str != "RGBA" && pixel_format_str != "BGRA") {
            fprintf(stderr, "Error: Invalid --pixel-format. Must be one of RGB, BGR, RGBA, BGRA.\n");
            return -1;
        }
         if (verbose) {
            fprintf(stderr, "Using pipe mode. Input: %dx%dx%d, Format: %s\n", input_width, input_height, input_channels, pixel_format_str.c_str());
        }
        set_binary_stdio();
    } else {
        if (inputpath.empty() || outputpath.empty())
        {
            print_usage();
            return -1;
        }
    }


    if (tilesize.size() != (gpuid.empty() ? 1 : gpuid.size()) && !tilesize.empty())
    {
        fprintf(stderr, "invalid tilesize argument\n");
        return -1;
    }

    for (int i=0; i<(int)tilesize.size(); i++)
    {
        if (tilesize[i] != 0 && tilesize[i] < 32)
        {
            fprintf(stderr, "invalid tilesize argument\n");
            return -1;
        }
    }

    if (!use_pipe_mode) { // Validations for non-pipe mode
        if (jobs_load < 1 || jobs_save < 1)
        {
            fprintf(stderr, "invalid thread count argument\n");
            return -1;
        }

        if (jobs_proc.size() != (gpuid.empty() ? 1 : gpuid.size()) && !jobs_proc.empty())
        {
            fprintf(stderr, "invalid jobs_proc thread count argument\n");
            return -1;
        }

        for (int i=0; i<(int)jobs_proc.size(); i++)
        {
            if (jobs_proc[i] < 1)
            {
                fprintf(stderr, "invalid jobs_proc thread count argument\n");
                return -1;
            }
        }

        if (queue_size < 1)
        {
            fprintf(stderr, "invalid queue size\n");
            return -1;
        }

        if (!path_is_directory(outputpath))
        {
            path_t ext = get_file_extension(outputpath);
            if (ext == PATHSTR("png") || ext == PATHSTR("PNG")) format = PATHSTR("png");
            else if (ext == PATHSTR("webp") || ext == PATHSTR("WEBP")) format = PATHSTR("webp");
            else if (ext == PATHSTR("jpg") || ext == PATHSTR("JPG") || ext == PATHSTR("jpeg") || ext == PATHSTR("JPEG")) format = PATHSTR("jpg");
            else {
                fprintf(stderr, "invalid outputpath extension type\n");
                return -1;
            }
        }

        if (format != PATHSTR("png") && format != PATHSTR("webp") && format != PATHSTR("jpg")) {
            fprintf(stderr, "invalid format argument\n");
            return -1;
        }
    }


    // collect input and output filepath for file mode
    std::vector<path_t> input_files;
    std::vector<path_t> output_files;
    if (!use_pipe_mode)
    {
        if (path_is_directory(inputpath) && path_is_directory(outputpath))
        {
            std::vector<path_t> filenames;
            int lr = list_directory(inputpath, filenames);
            if (lr != 0)
                return -1;

            const int count = filenames.size();
            input_files.resize(count);
            output_files.resize(count);

            path_t last_filename;
            path_t last_filename_noext;
            for (int i=0; i<count; i++)
            {
                path_t filename = filenames[i];
                path_t filename_noext = get_file_name_without_extension(filename);
                path_t output_filename = filename_noext + PATHSTR('.') + format;

                if (filename_noext == last_filename_noext) {
                    path_t output_filename2 = filename + PATHSTR('.') + format;
    #if _WIN32
                    fwprintf(stderr, L"both %ls and %ls output %ls ! %ls will output %ls\n", filename.c_str(), last_filename.c_str(), output_filename.c_str(), filename.c_str(), output_filename2.c_str());
    #else
                    fprintf(stderr, "both %s and %s output %s ! %s will output %s\n", filename.c_str(), last_filename.c_str(), output_filename.c_str(), filename.c_str(), output_filename2.c_str());
    #endif
                    output_filename = output_filename2;
                } else {
                    last_filename = filename;
                    last_filename_noext = filename_noext;
                }
                input_files[i] = inputpath + PATHSTR('/') + filename;
                output_files[i] = outputpath + PATHSTR('/') + output_filename;
            }
        }
        else if (!path_is_directory(inputpath) && !path_is_directory(outputpath))
        {
            input_files.push_back(inputpath);
            output_files.push_back(outputpath);
        }
        else
        {
            fprintf(stderr, "inputpath and outputpath must be either file or directory at the same time\n");
            return -1;
        }
    }

    int prepadding = 0;
    if (model.find(PATHSTR("models")) != path_t::npos || model.find(PATHSTR("models2")) != path_t::npos) {
        prepadding = 10;
    } else {
        fprintf(stderr, "unknown model dir type\n");
        // return -1; // Allow custom model paths for pipe mode, path check might not be relevant
    }


#if _WIN32
    wchar_t parampath_buf[256]; // Renamed to avoid conflict
    wchar_t modelpath_buf[256]; // Renamed to avoid conflict
    if (modelname == PATHSTR("realesr-animevideov3")) {
        swprintf(parampath_buf, 256, L"%s/%s-x%d.param", model.c_str(), modelname.c_str(), scale); // Use %d for int
        swprintf(modelpath_buf, 256, L"%s/%s-x%d.bin", model.c_str(), modelname.c_str(), scale);   // Use %d for int
    } else{
        swprintf(parampath_buf, 256, L"%s/%s.param", model.c_str(), modelname.c_str());
        swprintf(modelpath_buf, 256, L"%s/%s.bin", model.c_str(), modelname.c_str());
    }
#else
    char parampath_buf[256]; // Renamed
    char modelpath_buf[256]; // Renamed
    if (modelname == PATHSTR("realesr-animevideov3")) {
        sprintf(parampath_buf, "%s/%s-x%d.param", model.c_str(), modelname.c_str(), scale); // Use %d
        sprintf(modelpath_buf, "%s/%s-x%d.bin", model.c_str(), modelname.c_str(), scale);   // Use %d
    } else{
        sprintf(parampath_buf, "%s/%s.param", model.c_str(), modelname.c_str());
        sprintf(modelpath_buf, "%s/%s.bin", model.c_str(), modelname.c_str());
    }
#endif

    path_t paramfullpath = sanitize_filepath(parampath_buf);
    path_t modelfullpath = sanitize_filepath(modelpath_buf);

#if _WIN32
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

    ncnn::create_gpu_instance();

    if (gpuid.empty()) {
        gpuid.push_back(ncnn::get_default_gpu_index());
    }

    const int use_gpu_count = (int)gpuid.size();

    if (jobs_proc.empty() && !use_pipe_mode) {
        jobs_proc.resize(use_gpu_count, 2);
    }
    if (tilesize.empty()) {
        tilesize.resize(use_gpu_count, 0);
    }

    int cpu_count = std::max(1, ncnn::get_cpu_count());
    if(!use_pipe_mode){
        jobs_load = std::min(jobs_load, cpu_count);
        jobs_save = std::min(jobs_save, cpu_count);
    }


    int gpu_device_count = ncnn::get_gpu_count();
    for (int i=0; i<use_gpu_count; i++) {
        if (gpuid[i] < 0 || gpuid[i] >= gpu_device_count) { // Corrected variable name
            fprintf(stderr, "invalid gpu device\n");
            ncnn::destroy_gpu_instance();
            return -1;
        }
    }

    int total_jobs_proc = 0;
    if (!use_pipe_mode) {
        for (int i=0; i<use_gpu_count; i++) {
            int gpu_queue_count = ncnn::get_gpu_info(gpuid[i]).compute_queue_count();
            jobs_proc[i] = std::min(jobs_proc[i], gpu_queue_count);
            total_jobs_proc += jobs_proc[i];
        }
    } else { // Pipe mode typically uses 1 processing thread for the SR engine instance
        total_jobs_proc = 1;
        if(!jobs_proc.empty()) jobs_proc[0] = 1; else jobs_proc.push_back(1); // Ensure jobs_proc[0] is 1 for pipe mode
    }


    for (int i=0; i<use_gpu_count; i++) {
        if (tilesize[i] != 0)
            continue;
        uint32_t heap_budget = ncnn::get_gpu_device(gpuid[i])->get_heap_budget();
        if (model.find(PATHSTR("models")) != path_t::npos) { // Simplified tile size logic for pipe mode if needed
            if (heap_budget > 1900) tilesize[i] = 200;
            else if (heap_budget > 550) tilesize[i] = 100;
            else if (heap_budget > 190) tilesize[i] = 64;
            else tilesize[i] = 32;
        }
    }

    if (use_pipe_mode) {
        RealESRGAN* realesrgan_instance = new RealESRGAN(gpuid[0], tta_mode);
        realesrgan_instance->load(paramfullpath, modelfullpath);
        realesrgan_instance->scale = scale;
        realesrgan_instance->tilesize = tilesize[0];
        realesrgan_instance->prepadding = prepadding;

        if (verbose) {
            fprintf(stderr, "Pipe Mode: RealESRGAN initialized with: scale=%d, tilesize=%d, prepadding=%d, tta=%d, gpuid=%d\n",
                    realesrgan_instance->scale, realesrgan_instance->tilesize, realesrgan_instance->prepadding, tta_mode, gpuid[0]);
        }

        size_t frame_data_size = static_cast<size_t>(input_width) * input_height * input_channels;
        std::vector<unsigned char> frame_buffer(frame_data_size);
        if (verbose) fprintf(stderr, "Pipe Mode: Attempting to read %zu bytes from stdin for one frame...\n", frame_data_size);

        size_t bytes_read = fread(frame_buffer.data(), 1, frame_data_size, stdin);

        if (bytes_read == 0 && feof(stdin)) {
             if (verbose) fprintf(stderr, "Pipe Mode: EOF detected on stdin before reading any data. Exiting.\n");
        } else if (bytes_read != frame_data_size) {
            fprintf(stderr, "Error: Could not read full frame from stdin. Expected %zu, got %zu. feof: %d, ferror: %d\n",
                    frame_data_size, bytes_read, feof(stdin), ferror(stdin));
            delete realesrgan_instance;
            ncnn::destroy_gpu_instance();
            return -1;
        } else {
            if (verbose) fprintf(stderr, "Pipe Mode: Successfully read %zu bytes for a frame.\n", bytes_read);
            ncnn::Mat inimage;
            ncnn::Mat::PixelType pixel_type_ncnn_in = ncnn::Mat::PIXEL_BGR;
            if (input_channels == 3) {
                if (pixel_format_str == "RGB") pixel_type_ncnn_in = ncnn::Mat::PIXEL_RGB;
                else if (pixel_format_str == "BGR") pixel_type_ncnn_in = ncnn::Mat::PIXEL_BGR;
            } else if (input_channels == 4) {
                if (pixel_format_str == "RGBA") pixel_type_ncnn_in = ncnn::Mat::PIXEL_RGBA;
                else if (pixel_format_str == "BGRA") pixel_type_ncnn_in = ncnn::Mat::PIXEL_BGRA;
            }
            inimage = ncnn::Mat::from_pixels(frame_buffer.data(), pixel_type_ncnn_in, input_width, input_height);
            if(inimage.empty()){
                fprintf(stderr, "Error: ncnn::Mat::from_pixels failed for input.\n");
                delete realesrgan_instance; ncnn::destroy_gpu_instance(); return -1;
            }

            ncnn::Mat outimage = ncnn::Mat(input_width * scale, input_height * scale, (size_t)input_channels, input_channels);
             if(outimage.empty()){
                fprintf(stderr, "Error: ncnn::Mat allocation failed for output.\n");
                delete realesrgan_instance; ncnn::destroy_gpu_instance(); return -1;
            }

            if (verbose) fprintf(stderr, "Pipe Mode: Processing frame...\n");
            realesrgan_instance->process(inimage, outimage);

            std::vector<unsigned char> out_frame_buffer(outimage.total() * outimage.elempack);
            ncnn::Mat::PixelType pixel_type_ncnn_out = ncnn::Mat::PIXEL_BGR;
            if (outimage.elempack == 3) {
                if (pixel_format_str == "RGB") pixel_type_ncnn_out = ncnn::Mat::PIXEL_RGB;
                else if (pixel_format_str == "BGR") pixel_type_ncnn_out = ncnn::Mat::PIXEL_BGR;
            } else if (outimage.elempack == 4) {
                if (pixel_format_str == "RGBA") pixel_type_ncnn_out = ncnn::Mat::PIXEL_RGBA;
                else if (pixel_format_str == "BGRA") pixel_type_ncnn_out = ncnn::Mat::PIXEL_BGRA;
            }
            outimage.to_pixels(out_frame_buffer.data(), pixel_type_ncnn_out);

            if (verbose) fprintf(stderr, "Pipe Mode: Writing %zu bytes to stdout...\n", out_frame_buffer.size());
            size_t bytes_written = fwrite(out_frame_buffer.data(), 1, out_frame_buffer.size(), stdout);
            if(bytes_written != out_frame_buffer.size()){
                fprintf(stderr, "Error: Could not write full frame to stdout. Expected %zu, wrote %zu.\n", out_frame_buffer.size(), bytes_written);
            }
            fflush(stdout);
            if (verbose) fprintf(stderr, "Pipe Mode: Frame processed and written.\n");
        }
        delete realesrgan_instance;
    } else {
        // Original file processing path
        std::vector<RealESRGAN*> realesrgan(use_gpu_count);

        for (int i=0; i<use_gpu_count; i++) {
            realesrgan[i] = new RealESRGAN(gpuid[i], tta_mode);
            realesrgan[i]->load(paramfullpath, modelfullpath);
            realesrgan[i]->scale = scale;
            realesrgan[i]->tilesize = tilesize[i];
            realesrgan[i]->prepadding = prepadding;
        }

        { // Main routine block for file processing
            LoadThreadParams ltp;
            ltp.scale = scale; // This was missing for RealESRGAN load params
            ltp.jobs_load = jobs_load;
            ltp.input_files = input_files;
            ltp.output_files = output_files;
            ncnn::Thread load_thread(load, (void*)&ltp);

            std::vector<ProcThreadParams> ptp(use_gpu_count);
            for (int i=0; i<use_gpu_count; i++) {
                ptp[i].realesrgan = realesrgan[i];
            }

            std::vector<ncnn::Thread*> proc_threads(total_jobs_proc);
            {
                int total_jobs_proc_id = 0;
                for (int i=0; i<use_gpu_count; i++) {
                    for (int j=0; j<jobs_proc[i]; j++) { // This was jobs_proc[i] for RealCUGAN, should be consistent or adapted
                        proc_threads[total_jobs_proc_id++] = new ncnn::Thread(proc, (void*)&ptp[i]);
                    }
                }
            }

            SaveThreadParams stp;
            stp.verbose = verbose;
            std::vector<ncnn::Thread*> save_threads(jobs_save);
            for (int i=0; i<jobs_save; i++) {
                save_threads[i] = new ncnn::Thread(save, (void*)&stp);
            }

            load_thread.join();
            Task end_task; // Renamed from 'end'
            end_task.id = -233;
            for (int i=0; i<total_jobs_proc; i++) {
                toproc.put(end_task);
            }
            for (int i=0; i<total_jobs_proc; i++) {
                proc_threads[i]->join();
                delete proc_threads[i];
            }
            for (int i=0; i<jobs_save; i++) {
                tosave.put(end_task);
            }
            for (int i=0; i<jobs_save; i++) {
                save_threads[i]->join();
                delete save_threads[i];
            }
        }

        for (int i=0; i<use_gpu_count; i++) {
            delete realesrgan[i];
        }
        realesrgan.clear();
    }

    ncnn::destroy_gpu_instance();

    return 0;
}

class ProcThreadParams
{
public:
    const RealESRGAN* realesrgan;
};

void* proc(void* args)
{
    const ProcThreadParams* ptp = (const ProcThreadParams*)args;
    const RealESRGAN* realesrgan = ptp->realesrgan;

    for (;;)
    {
        Task v;

        toproc.get(v);

        if (v.id == -233)
            break;

        realesrgan->process(v.inimage, v.outimage);

        tosave.put(v);
    }

    return 0;
}

class SaveThreadParams
{
public:
    int verbose;
};

void* save(void* args)
{
    const SaveThreadParams* stp = (const SaveThreadParams*)args;
    const int verbose = stp->verbose;

    for (;;)
    {
        Task v;

        tosave.get(v);

        if (v.id == -233)
            break;

        // free input pixel data
        {
            unsigned char* pixeldata = (unsigned char*)v.inimage.data;
            if (v.webp == 1)
            {
                free(pixeldata);
            }
            else
            {
#if _WIN32
                free(pixeldata);
#else
                stbi_image_free(pixeldata);
#endif
            }
        }

        int success = 0;

        path_t ext = get_file_extension(v.outpath);

        /* ----------- Create folder if not exists -------------------*/
        fs::path fs_path = fs::absolute(v.outpath);
        std::string parent_path = fs_path.parent_path().string();
        if (fs::exists(parent_path) != 1){
            std::cout << "Create folder: [" << parent_path << "]." << std::endl;
            fs::create_directories(parent_path);
        }

        if (ext == PATHSTR("webp") || ext == PATHSTR("WEBP"))
        {
            success = webp_save(v.outpath.c_str(), v.outimage.w, v.outimage.h, v.outimage.elempack, (const unsigned char*)v.outimage.data);
        }
        else if (ext == PATHSTR("png") || ext == PATHSTR("PNG"))
        {
#if _WIN32
            success = wic_encode_image(v.outpath.c_str(), v.outimage.w, v.outimage.h, v.outimage.elempack, v.outimage.data);
#else
            success = stbi_write_png(v.outpath.c_str(), v.outimage.w, v.outimage.h, v.outimage.elempack, v.outimage.data, 0);
#endif
        }
        else if (ext == PATHSTR("jpg") || ext == PATHSTR("JPG") || ext == PATHSTR("jpeg") || ext == PATHSTR("JPEG"))
        {
#if _WIN32
            success = wic_encode_jpeg_image(v.outpath.c_str(), v.outimage.w, v.outimage.h, v.outimage.elempack, v.outimage.data);
#else
            success = stbi_write_jpg(v.outpath.c_str(), v.outimage.w, v.outimage.h, v.outimage.elempack, v.outimage.data, 100);
#endif
        }
        if (success)
        {
            if (verbose)
            {
#if _WIN32
                fwprintf(stderr, L"%ls -> %ls done\n", v.inpath.c_str(), v.outpath.c_str());
#else
                fprintf(stderr, "%s -> %s done\n", v.inpath.c_str(), v.outpath.c_str());
#endif
            }
        }
        else
        {
#if _WIN32
            fwprintf(stderr, L"encode image %ls failed\n", v.outpath.c_str());
#else
            fprintf(stderr, "encode image %s failed\n", v.outpath.c_str());
#endif
        }
    }

    return 0;
}


#if _WIN32
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    path_t inputpath;
    path_t outputpath;
    int scale = 4;
    std::vector<int> tilesize;
    path_t model = PATHSTR("models");
    path_t modelname = PATHSTR("realesr-animevideov3");
    std::vector<int> gpuid;
    int jobs_load = 1;
    std::vector<int> jobs_proc;
    int jobs_save = 2;
    int verbose = 0;
    int tta_mode = 0;
    path_t format = PATHSTR("png");

    // New flags for pipe mode
    bool use_pipe_mode = false;
    int input_width = 0;
    int input_height = 0;
    int input_channels = 0;
    std::string pixel_format_str = "BGR"; // Default

#if _WIN32
    setlocale(LC_ALL, "");
    // Manual parsing for long options first
    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"--use-pipe") == 0) {
            use_pipe_mode = true;
        } else if (wcscmp(argv[i], L"--input-width") == 0 && i + 1 < argc) {
            input_width = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--input-height") == 0 && i + 1 < argc) {
            input_height = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--input-channels") == 0 && i + 1 < argc) {
            input_channels = _wtoi(argv[++i]);
        } else if (wcscmp(argv[i], L"--pixel-format") == 0 && i + 1 < argc) {
            wchar_t* format_wstr = argv[++i];
            char format_cstr[256];
            size_t num_converted;
            wcstombs_s(&num_converted, format_cstr, sizeof(format_cstr), format_wstr, _TRUNCATE);
            if (num_converted > 0) {
                pixel_format_str = format_cstr;
            }
        }
    }
    wchar_t opt;
    while ((opt = getopt(argc, argv, L"i:o:s:t:m:n:g:j:f:q:vxh")) != (wchar_t)-1)
    {
        switch (opt)
        {
        case L'i':
            inputpath = optarg;
            break;
        case L'o':
            outputpath = optarg;
            break;
        case L's':
            scale = _wtoi(optarg);
            break;
        case L't':
            tilesize = parse_optarg_int_array(optarg);
            break;
        case L'm':
            model = optarg;
            break;
        case L'n':
            modelname = optarg;
            break;
        case L'g':
            gpuid = parse_optarg_int_array(optarg);
            break;
        case L'j':
            swscanf(optarg, L"%d:%*[^:]:%d", &jobs_load, &jobs_save);
            jobs_proc = parse_optarg_int_array(wcschr(optarg, L':') + 1);
            break;
        case L'f':
            format = optarg;
            break;
        case L'q':
            queue_size = _wtoi(optarg);
            break;
        case L'v':
            verbose = 1;
            break;
        case L'x':
            tta_mode = 1;
            break;
        case L'h':
        default:
            print_usage();
            return -1;
        }
    }
#else // _WIN32
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--use-pipe") == 0) {
            use_pipe_mode = true;
        } else if (strcmp(argv[i], "--input-width") == 0 && i + 1 < argc) {
            input_width = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--input-height") == 0 && i + 1 < argc) {
            input_height = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--input-channels") == 0 && i + 1 < argc) {
            input_channels = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--pixel-format") == 0 && i + 1 < argc) {
            pixel_format_str = argv[++i];
        }
    }
    int opt;
    while ((opt = getopt(argc, argv, "i:o:s:t:m:n:g:j:f:q:vxh")) != -1)
    {
        switch (opt)
        {
        case 'i':
            inputpath = optarg;
            break;
        case 'o':
            outputpath = optarg;
            break;
        case 's':
            scale = atoi(optarg);
            break;
        case 't':
            tilesize = parse_optarg_int_array(optarg);
            break;
        case 'm':
            model = optarg;
            break;
        case 'n':
            modelname = optarg;
            break;
        case 'g':
            gpuid = parse_optarg_int_array(optarg);
            break;
        case 'j':
            sscanf(optarg, "%d:%*[^:]:%d", &jobs_load, &jobs_save);
            jobs_proc = parse_optarg_int_array(strchr(optarg, ':') + 1);
            break;
        case 'f':
            format = optarg;
            break;
        case 'q':
            queue_size = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'x':
            tta_mode = 1;
            break;
        case 'h':
        default:
            print_usage();
            return -1;
        }
    }
#endif // _WIN32

    if (inputpath.empty() || outputpath.empty())
    {
        print_usage();
        return -1;
    }

    if (tilesize.size() != (gpuid.empty() ? 1 : gpuid.size()) && !tilesize.empty())
    {
        fprintf(stderr, "invalid tilesize argument\n");
        return -1;
    }

    for (int i=0; i<(int)tilesize.size(); i++)
    {
        if (tilesize[i] != 0 && tilesize[i] < 32)
        {
            fprintf(stderr, "invalid tilesize argument\n");
            return -1;
        }
    }

    if (jobs_load < 1 || jobs_save < 1)
    {
        fprintf(stderr, "invalid thread count argument\n");
        return -1;
    }

    if (jobs_proc.size() != (gpuid.empty() ? 1 : gpuid.size()) && !jobs_proc.empty())
    {
        fprintf(stderr, "invalid jobs_proc thread count argument\n");
        return -1;
    }

    for (int i=0; i<(int)jobs_proc.size(); i++)
    {
        if (jobs_proc[i] < 1)
        {
            fprintf(stderr, "invalid jobs_proc thread count argument\n");
            return -1;
        }
    }

    if (queue_size < 1)
    {
        fprintf(stderr, "invalid queue size\n");
        return -1;
    }

    if (!path_is_directory(outputpath))
    {
        // guess format from outputpath no matter what format argument specified
        path_t ext = get_file_extension(outputpath);

        if (ext == PATHSTR("png") || ext == PATHSTR("PNG"))
        {
            format = PATHSTR("png");
        }
        else if (ext == PATHSTR("webp") || ext == PATHSTR("WEBP"))
        {
            format = PATHSTR("webp");
        }
        else if (ext == PATHSTR("jpg") || ext == PATHSTR("JPG") || ext == PATHSTR("jpeg") || ext == PATHSTR("JPEG"))
        {
            format = PATHSTR("jpg");
        }
        else
        {
            fprintf(stderr, "invalid outputpath extension type\n");
            return -1;
        }
    }

    if (format != PATHSTR("png") && format != PATHSTR("webp") && format != PATHSTR("jpg"))
    {
        fprintf(stderr, "invalid format argument\n");
        return -1;
    }

    // collect input and output filepath
    std::vector<path_t> input_files;
    std::vector<path_t> output_files;
    {
        if (path_is_directory(inputpath) && path_is_directory(outputpath))
        {
            std::vector<path_t> filenames;
            int lr = list_directory(inputpath, filenames);
            if (lr != 0)
                return -1;

            const int count = filenames.size();
            input_files.resize(count);
            output_files.resize(count);

            path_t last_filename;
            path_t last_filename_noext;
            for (int i=0; i<count; i++)
            {
                path_t filename = filenames[i];
                path_t filename_noext = get_file_name_without_extension(filename);
                path_t output_filename = filename_noext + PATHSTR('.') + format;

                // filename list is sorted, check if output image path conflicts
                if (filename_noext == last_filename_noext)
                {
                    path_t output_filename2 = filename + PATHSTR('.') + format;
#if _WIN32
                    fwprintf(stderr, L"both %ls and %ls output %ls ! %ls will output %ls\n", filename.c_str(), last_filename.c_str(), output_filename.c_str(), filename.c_str(), output_filename2.c_str());
#else
                    fprintf(stderr, "both %s and %s output %s ! %s will output %s\n", filename.c_str(), last_filename.c_str(), output_filename.c_str(), filename.c_str(), output_filename2.c_str());
#endif
                    output_filename = output_filename2;
                }
                else
                {
                    last_filename = filename;
                    last_filename_noext = filename_noext;
                }

                input_files[i] = inputpath + PATHSTR('/') + filename;
                output_files[i] = outputpath + PATHSTR('/') + output_filename;
            }
        }
        else if (!path_is_directory(inputpath) && !path_is_directory(outputpath))
        {
            input_files.push_back(inputpath);
            output_files.push_back(outputpath);
        }
        else
        {
            fprintf(stderr, "inputpath and outputpath must be either file or directory at the same time\n");
            return -1;
        }
    }

    int prepadding = 0;

    if (model.find(PATHSTR("models")) != path_t::npos
        || model.find(PATHSTR("models2")) != path_t::npos)
    {
        prepadding = 10;
    }
    else
    {
        fprintf(stderr, "unknown model dir type\n");
        return -1;
    }

    // if (modelname.find(PATHSTR("realesrgan-x4plus")) != path_t::npos
    //     || modelname.find(PATHSTR("realesrnet-x4plus")) != path_t::npos
    //     || modelname.find(PATHSTR("esrgan-x4")) != path_t::npos)
    // {}
    // else
    // {
    //     fprintf(stderr, "unknown model name\n");
    //     return -1;
    // }

#if _WIN32
    wchar_t parampath[256];
    wchar_t modelpath[256];

    if (modelname == PATHSTR("realesr-animevideov3"))
    {
        swprintf(parampath, 256, L"%s/%s-x%s.param", model.c_str(), modelname.c_str(), std::to_string(scale));
        swprintf(modelpath, 256, L"%s/%s-x%s.bin", model.c_str(), modelname.c_str(), std::to_string(scale));
    }
    else{
        swprintf(parampath, 256, L"%s/%s.param", model.c_str(), modelname.c_str());
        swprintf(modelpath, 256, L"%s/%s.bin", model.c_str(), modelname.c_str());
    }

#else
    char parampath[256];
    char modelpath[256];

    if (modelname == PATHSTR("realesr-animevideov3"))
    {
        sprintf(parampath, "%s/%s-x%s.param", model.c_str(), modelname.c_str(), std::to_string(scale).c_str());
        sprintf(modelpath, "%s/%s-x%s.bin", model.c_str(), modelname.c_str(), std::to_string(scale).c_str());
    }
    else{
        sprintf(parampath, "%s/%s.param", model.c_str(), modelname.c_str());
        sprintf(modelpath, "%s/%s.bin", model.c_str(), modelname.c_str());
    }
#endif

    path_t paramfullpath = sanitize_filepath(parampath);
    path_t modelfullpath = sanitize_filepath(modelpath);

#if _WIN32
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

    ncnn::create_gpu_instance();

    if (gpuid.empty())
    {
        gpuid.push_back(ncnn::get_default_gpu_index());
    }

    const int use_gpu_count = (int)gpuid.size();

    if (jobs_proc.empty())
    {
        jobs_proc.resize(use_gpu_count, 2);
    }

    if (tilesize.empty())
    {
        tilesize.resize(use_gpu_count, 0);
    }

    int cpu_count = std::max(1, ncnn::get_cpu_count());
    jobs_load = std::min(jobs_load, cpu_count);
    jobs_save = std::min(jobs_save, cpu_count);

    int gpu_count = ncnn::get_gpu_count();
    for (int i=0; i<use_gpu_count; i++)
    {
        if (gpuid[i] < 0 || gpuid[i] >= gpu_count)
        {
            fprintf(stderr, "invalid gpu device\n");

            ncnn::destroy_gpu_instance();
            return -1;
        }
    }

    int total_jobs_proc = 0;
    for (int i=0; i<use_gpu_count; i++)
    {
        int gpu_queue_count = ncnn::get_gpu_info(gpuid[i]).compute_queue_count();
        jobs_proc[i] = std::min(jobs_proc[i], gpu_queue_count);
        total_jobs_proc += jobs_proc[i];
    }

    for (int i=0; i<use_gpu_count; i++)
    {
        if (tilesize[i] != 0)
            continue;

        uint32_t heap_budget = ncnn::get_gpu_device(gpuid[i])->get_heap_budget();

        // more fine-grained tilesize policy here
        if (model.find(PATHSTR("models")) != path_t::npos)
        {
            if (heap_budget > 1900)
                tilesize[i] = 200;
            else if (heap_budget > 550)
                tilesize[i] = 100;
            else if (heap_budget > 190)
                tilesize[i] = 64;
            else
                tilesize[i] = 32;
        }
    }

    {
        std::vector<RealESRGAN*> realesrgan(use_gpu_count);

        for (int i=0; i<use_gpu_count; i++)
        {
            realesrgan[i] = new RealESRGAN(gpuid[i], tta_mode);

            realesrgan[i]->load(paramfullpath, modelfullpath);

            realesrgan[i]->scale = scale;
            realesrgan[i]->tilesize = tilesize[i];
            realesrgan[i]->prepadding = prepadding;
        }

        // main routine
        {
            // load image
            LoadThreadParams ltp;
            ltp.scale = scale;
            ltp.jobs_load = jobs_load;
            ltp.input_files = input_files;
            ltp.output_files = output_files;

            ncnn::Thread load_thread(load, (void*)&ltp);

            // realesrgan proc
            std::vector<ProcThreadParams> ptp(use_gpu_count);
            for (int i=0; i<use_gpu_count; i++)
            {
                ptp[i].realesrgan = realesrgan[i];
            }

            std::vector<ncnn::Thread*> proc_threads(total_jobs_proc);
            {
                int total_jobs_proc_id = 0;
                for (int i=0; i<use_gpu_count; i++)
                {
                    for (int j=0; j<jobs_proc[i]; j++)
                    {
                        proc_threads[total_jobs_proc_id++] = new ncnn::Thread(proc, (void*)&ptp[i]);
                    }
                }
            }

            // save image
            SaveThreadParams stp;
            stp.verbose = verbose;

            std::vector<ncnn::Thread*> save_threads(jobs_save);
            for (int i=0; i<jobs_save; i++)
            {
                save_threads[i] = new ncnn::Thread(save, (void*)&stp);
            }

            // end
            load_thread.join();

            Task end;
            end.id = -233;

            for (int i=0; i<total_jobs_proc; i++)
            {
                toproc.put(end);
            }

            for (int i=0; i<total_jobs_proc; i++)
            {
                proc_threads[i]->join();
                delete proc_threads[i];
            }

            for (int i=0; i<jobs_save; i++)
            {
                tosave.put(end);
            }

            for (int i=0; i<jobs_save; i++)
            {
                save_threads[i]->join();
                delete save_threads[i];
            }
        }

        for (int i=0; i<use_gpu_count; i++)
        {
            delete realesrgan[i];
        }
        realesrgan.clear();
    }

    ncnn::destroy_gpu_instance();

    return 0;
}
