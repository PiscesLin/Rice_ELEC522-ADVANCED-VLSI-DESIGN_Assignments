#include <systemc>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <stdint.h>
#include "SysCFileHandler.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include <complex>
#include <stdbool.h>
#include "autopilot_cbe.h"
#include "hls_stream.h"
#include "hls_half.h"
#include "hls_signal_handler.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// wrapc file define:
#define AUTOTB_TVIN_x "../tv/cdatafile/c.cordic.autotvin_x.dat"
#define AUTOTB_TVOUT_x "../tv/cdatafile/c.cordic.autotvout_x.dat"
#define AUTOTB_TVIN_y "../tv/cdatafile/c.cordic.autotvin_y.dat"
#define AUTOTB_TVOUT_y "../tv/cdatafile/c.cordic.autotvout_y.dat"
#define AUTOTB_TVIN_theta "../tv/cdatafile/c.cordic.autotvin_theta.dat"
#define AUTOTB_TVOUT_theta "../tv/cdatafile/c.cordic.autotvout_theta.dat"
#define AUTOTB_TVIN_c "../tv/cdatafile/c.cordic.autotvin_c.dat"
#define AUTOTB_TVOUT_c "../tv/cdatafile/c.cordic.autotvout_c.dat"
#define AUTOTB_TVIN_s "../tv/cdatafile/c.cordic.autotvin_s.dat"
#define AUTOTB_TVOUT_s "../tv/cdatafile/c.cordic.autotvout_s.dat"
#define AUTOTB_TVIN_arctan "../tv/cdatafile/c.cordic.autotvin_arctan.dat"
#define AUTOTB_TVOUT_arctan "../tv/cdatafile/c.cordic.autotvout_arctan.dat"
#define AUTOTB_TVIN_mode "../tv/cdatafile/c.cordic.autotvin_mode.dat"
#define AUTOTB_TVOUT_mode "../tv/cdatafile/c.cordic.autotvout_mode.dat"

#define INTER_TCL "../tv/cdatafile/ref.tcl"

// tvout file define:
#define AUTOTB_TVOUT_PC_x "../tv/rtldatafile/rtl.cordic.autotvout_x.dat"
#define AUTOTB_TVOUT_PC_y "../tv/rtldatafile/rtl.cordic.autotvout_y.dat"
#define AUTOTB_TVOUT_PC_theta "../tv/rtldatafile/rtl.cordic.autotvout_theta.dat"
#define AUTOTB_TVOUT_PC_c "../tv/rtldatafile/rtl.cordic.autotvout_c.dat"
#define AUTOTB_TVOUT_PC_s "../tv/rtldatafile/rtl.cordic.autotvout_s.dat"
#define AUTOTB_TVOUT_PC_arctan "../tv/rtldatafile/rtl.cordic.autotvout_arctan.dat"
#define AUTOTB_TVOUT_PC_mode "../tv/rtldatafile/rtl.cordic.autotvout_mode.dat"


static const bool little_endian()
{
  int a = 1;
  return *(char*)&a == 1;
}

inline static void rev_endian(char* p, size_t nbytes)
{
  std::reverse(p, p+nbytes);
}

template<size_t bit_width>
struct transaction {
  typedef uint64_t depth_t;
  static const size_t wbytes = (bit_width+7)>>3;
  static const size_t dbytes = sizeof(depth_t);
  const depth_t depth;
  const size_t vbytes;
  const size_t tbytes;
  char * const p;
  typedef char (*p_dat)[wbytes];
  p_dat vp;

  transaction(depth_t depth)
    : depth(depth), vbytes(wbytes*depth), tbytes(dbytes+vbytes),
      p(new char[tbytes]) {
    *(depth_t*)p = depth;
    rev_endian(p, dbytes);
    vp = (p_dat) (p+dbytes);
  }

  void reorder() {
    rev_endian(p, dbytes);
    p_dat vp = (p_dat) (p+dbytes);
    for (depth_t i = 0; i < depth; ++i) {
      rev_endian(vp[i], wbytes);
    }
  }

  template<size_t psize>
  void import(char* param, depth_t num, int64_t offset) {
    param -= offset*psize;
    for (depth_t i = 0; i < num; ++i) {
      memcpy(vp[i], param, wbytes);
      param += psize;
      if (little_endian()) {
        rev_endian(vp[i], wbytes);
      }
    }
    vp += num;
  }

  template<size_t psize>
  void send(char* param, depth_t num) {
    for (depth_t i = 0; i < num; ++i) {
      memcpy(param, vp[i], wbytes);
      param += psize;
    }
    vp += num;
  }

  template<size_t psize>
  void send(char* param, depth_t num, int64_t skip) {
    for (depth_t i = 0; i < num; ++i) {
      memcpy(param, vp[skip+i], wbytes);
      param += psize;
    }
  }

  ~transaction() { if (p) { delete[] p; } }
};


inline static const std::string begin_str(int num)
{
  return std::string("[[transaction]]           ")
         .append(std::to_string(num))
         .append("\n");
}

inline static const std::string end_str()
{
  return std::string("[[/transaction]]\n");
}

const std::string formatData(unsigned char *pos, size_t wbits)
{
  bool LE = little_endian();
  size_t wbytes = (wbits+7)>>3;
  size_t i = LE ? wbytes-1 : 0;
  auto next = [&] () {
    auto c = pos[i];
    LE ? --i : ++i;
    return c;
  };
  std::ostringstream ss;
  ss << "0x";
  if (int t = (wbits & 0x7)) {
    if (t <= 4) {
      unsigned char mask = (1<<t)-1;
      ss << std::hex << std::setfill('0') << std::setw(1)
         << (int) (next() & mask);
      wbytes -= 1;
    }
  }
  for (size_t i = 0; i < wbytes; ++i) {
    ss << std::hex << std::setfill('0') << std::setw(2) << (int)next();
  }
  ss.put('\n');
  return ss.str();
}

static bool RTLOutputCheckAndReplacement(std::string &data)
{
  bool changed = false;
  for (size_t i = 2; i < data.size(); ++i) {
    if (data[i] == 'X' || data[i] == 'x') {
      data[i] = '0';
      changed = true;
    }
  }
  return changed;
}

struct SimException : public std::exception {
  const char *msg;
  const size_t line;
  SimException(const char *msg, const size_t line)
    : msg(msg), line(line)
  {
  }
};

template<size_t bit_width>
class PostCheck
{
  static const char *bad;
  static const char *err;
  std::fstream stream;
  std::string s;

  void send(char *p, ap_uint<bit_width> &data, size_t l, size_t rest)
  {
    if (rest == 0) {
      if (!little_endian()) {
        const size_t wbytes = (bit_width+7)>>3;
        rev_endian(p-wbytes, wbytes);
      }
    } else if (rest < 8) {
      *p = data.range(l+rest-1, l).to_uint();
      send(p+1, data, l+rest, 0);
    } else {
      *p = data.range(l+8-1, l).to_uint();
      send(p+1, data, l+8, rest-8);
    }
  }

  void readline()
  {
    std::getline(stream, s);
    if (stream.eof()) {
      throw SimException(bad, __LINE__);
    }
  }

public:
  char *param;
  size_t psize;
  size_t depth;

  PostCheck(const char *file)
  {
    stream.open(file);
    if (stream.fail()) {
      throw SimException(err, __LINE__);
    } else {
      readline();
      if (s != "[[[runtime]]]") {
        throw SimException(bad, __LINE__);
      }
    }
  }

  ~PostCheck() noexcept(false)
  {
    stream.close();
  }

  void run(size_t AESL_transaction_pc, size_t skip)
  {
    if (stream.peek() == '[') {
      readline();
    }

    for (size_t i = 0; i < skip; ++i) {
      readline();
    }

    bool foundX = false;
    for (size_t i = 0; i < depth; ++i) {
      readline();
      foundX |= RTLOutputCheckAndReplacement(s);
      ap_uint<bit_width> data(s.c_str(), 16);
      send(param+i*psize, data, 0, bit_width);
    }
    if (foundX) {
      std::cerr << "WARNING: [SIM 212-201] RTL produces unknown value "
                << "'x' or 'X' on some port, possible cause: "
                << "There are uninitialized variables in the design.\n";
    }

    if (stream.peek() == '[') {
      readline();
    }
  }
};

template<size_t bit_width>
const char* PostCheck<bit_width>::bad = "Bad TV file";

template<size_t bit_width>
const char* PostCheck<bit_width>::err = "Error on TV file";
      
class INTER_TCL_FILE {
  public:
INTER_TCL_FILE(const char* name) {
  mName = name; 
  x_depth = 0;
  y_depth = 0;
  theta_depth = 0;
  c_depth = 0;
  s_depth = 0;
  arctan_depth = 0;
  mode_depth = 0;
  trans_num =0;
}
~INTER_TCL_FILE() {
  mFile.open(mName);
  if (!mFile.good()) {
    cout << "Failed to open file ref.tcl" << endl;
    exit (1); 
  }
  string total_list = get_depth_list();
  mFile << "set depth_list {\n";
  mFile << total_list;
  mFile << "}\n";
  mFile << "set trans_num "<<trans_num<<endl;
  mFile.close();
}
string get_depth_list () {
  stringstream total_list;
  total_list << "{x " << x_depth << "}\n";
  total_list << "{y " << y_depth << "}\n";
  total_list << "{theta " << theta_depth << "}\n";
  total_list << "{c " << c_depth << "}\n";
  total_list << "{s " << s_depth << "}\n";
  total_list << "{arctan " << arctan_depth << "}\n";
  total_list << "{mode " << mode_depth << "}\n";
  return total_list.str();
}
void set_num (int num , int* class_num) {
  (*class_num) = (*class_num) > num ? (*class_num) : num;
}
void set_string(std::string list, std::string* class_list) {
  (*class_list) = list;
}
  public:
    int x_depth;
    int y_depth;
    int theta_depth;
    int c_depth;
    int s_depth;
    int arctan_depth;
    int mode_depth;
    int trans_num;
  private:
    ofstream mFile;
    const char* mName;
};


extern "C" void cordic_hw_stub_wrapper(volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *, volatile void *);

extern "C" void apatb_cordic_hw(volatile void * __xlx_apatb_param_x, volatile void * __xlx_apatb_param_y, volatile void * __xlx_apatb_param_theta, volatile void * __xlx_apatb_param_c, volatile void * __xlx_apatb_param_s, volatile void * __xlx_apatb_param_arctan, volatile void * __xlx_apatb_param_mode) {
  refine_signal_handler();
  fstream wrapc_switch_file_token;
  wrapc_switch_file_token.open(".hls_cosim_wrapc_switch.log");
static AESL_FILE_HANDLER aesl_fh;
  int AESL_i;
  if (wrapc_switch_file_token.good())
  {

    CodeState = ENTER_WRAPC_PC;
    static unsigned AESL_transaction_pc = 0;
    string AESL_token;
    string AESL_num;
{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_theta);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<16> > theta_pc_buffer(1);
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              theta_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "theta" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {((char*)__xlx_apatb_param_theta)[0*2+0] = theta_pc_buffer[0].range(7, 0).to_int64();
((char*)__xlx_apatb_param_theta)[0*2+1] = theta_pc_buffer[0].range(15, 8).to_int64();
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  {
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_c);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<16> > c_pc_buffer(1);
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              c_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "c" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {((char*)__xlx_apatb_param_c)[0*2+0] = c_pc_buffer[0].range(7, 0).to_int64();
((char*)__xlx_apatb_param_c)[0*2+1] = c_pc_buffer[0].range(15, 8).to_int64();
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  {
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_s);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<16> > s_pc_buffer(1);
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              s_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "s" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {((char*)__xlx_apatb_param_s)[0*2+0] = s_pc_buffer[0].range(7, 0).to_int64();
((char*)__xlx_apatb_param_s)[0*2+1] = s_pc_buffer[0].range(15, 8).to_int64();
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  {
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_arctan);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<16> > arctan_pc_buffer(1);
          int i = 0;
          bool has_unknown_value = false;
          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            has_unknown_value |= RTLOutputCheckAndReplacement(AESL_token);
  
            // push token into output port buffer
            if (AESL_token != "") {
              arctan_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (has_unknown_value) {
            cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' or 'X' on port " 
                 << "arctan" << ", possible cause: There are uninitialized variables in the C design."
                 << endl; 
          }
  
          if (i > 0) {((char*)__xlx_apatb_param_arctan)[0*2+0] = arctan_pc_buffer[0].range(7, 0).to_int64();
((char*)__xlx_apatb_param_arctan)[0*2+1] = arctan_pc_buffer[0].range(15, 8).to_int64();
}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
    AESL_transaction_pc++;
    return ;
  }
static unsigned AESL_transaction;
static INTER_TCL_FILE tcl_file(INTER_TCL);
std::vector<char> __xlx_sprintf_buffer(1024);
CodeState = ENTER_WRAPC;
CodeState = DUMP_INPUTS;
// print x Transactions
{
aesl_fh.write(AUTOTB_TVIN_x, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_x;
aesl_fh.write(AUTOTB_TVIN_x, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.x_depth);
aesl_fh.write(AUTOTB_TVIN_x, end_str());
}

// print y Transactions
{
aesl_fh.write(AUTOTB_TVIN_y, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_y;
aesl_fh.write(AUTOTB_TVIN_y, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.y_depth);
aesl_fh.write(AUTOTB_TVIN_y, end_str());
}

// print theta Transactions
{
aesl_fh.write(AUTOTB_TVIN_theta, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_theta;
aesl_fh.write(AUTOTB_TVIN_theta, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.theta_depth);
aesl_fh.write(AUTOTB_TVIN_theta, end_str());
}

// print c Transactions
{
aesl_fh.write(AUTOTB_TVIN_c, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_c;
aesl_fh.write(AUTOTB_TVIN_c, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.c_depth);
aesl_fh.write(AUTOTB_TVIN_c, end_str());
}

// print s Transactions
{
aesl_fh.write(AUTOTB_TVIN_s, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_s;
aesl_fh.write(AUTOTB_TVIN_s, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.s_depth);
aesl_fh.write(AUTOTB_TVIN_s, end_str());
}

// print arctan Transactions
{
aesl_fh.write(AUTOTB_TVIN_arctan, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_arctan;
aesl_fh.write(AUTOTB_TVIN_arctan, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.arctan_depth);
aesl_fh.write(AUTOTB_TVIN_arctan, end_str());
}

// print mode Transactions
{
aesl_fh.write(AUTOTB_TVIN_mode, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_mode;
aesl_fh.write(AUTOTB_TVIN_mode, formatData(pos, 1));
}
  tcl_file.set_num(1, &tcl_file.mode_depth);
aesl_fh.write(AUTOTB_TVIN_mode, end_str());
}

CodeState = CALL_C_DUT;
cordic_hw_stub_wrapper(__xlx_apatb_param_x, __xlx_apatb_param_y, __xlx_apatb_param_theta, __xlx_apatb_param_c, __xlx_apatb_param_s, __xlx_apatb_param_arctan, __xlx_apatb_param_mode);
CodeState = DUMP_OUTPUTS;
// print theta Transactions
{
aesl_fh.write(AUTOTB_TVOUT_theta, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_theta;
aesl_fh.write(AUTOTB_TVOUT_theta, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.theta_depth);
aesl_fh.write(AUTOTB_TVOUT_theta, end_str());
}

// print c Transactions
{
aesl_fh.write(AUTOTB_TVOUT_c, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_c;
aesl_fh.write(AUTOTB_TVOUT_c, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.c_depth);
aesl_fh.write(AUTOTB_TVOUT_c, end_str());
}

// print s Transactions
{
aesl_fh.write(AUTOTB_TVOUT_s, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_s;
aesl_fh.write(AUTOTB_TVOUT_s, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.s_depth);
aesl_fh.write(AUTOTB_TVOUT_s, end_str());
}

// print arctan Transactions
{
aesl_fh.write(AUTOTB_TVOUT_arctan, begin_str(AESL_transaction));
{
auto *pos = (unsigned char*)__xlx_apatb_param_arctan;
aesl_fh.write(AUTOTB_TVOUT_arctan, formatData(pos, 16));
}
  tcl_file.set_num(1, &tcl_file.arctan_depth);
aesl_fh.write(AUTOTB_TVOUT_arctan, end_str());
}

CodeState = DELETE_CHAR_BUFFERS;
AESL_transaction++;
tcl_file.set_num(AESL_transaction , &tcl_file.trans_num);
}
