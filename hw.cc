#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <map>
using namespace std;
string file_name = "input.sal";


// function that test if the addition produce overflow
bool addition_is_overflow(signed int A, signed int B) {
   if (A > 0) {
    return B > (INT_MAX - A);
}
if (A < 0) {
    return B < (INT_MIN - A);
}
return false;
}

// implementing singelton pattern to Hardware class
class Hardware{
  private:
    static Hardware* hardware_instance;
    map<char, string> memory; 
    int RegisterA;
    int RegisterB;
    int PC;
    int zero_result_bit;
    int over_flow_bit;
    Hardware(){};
    Hardware(Hardware const&);
    void operator=(Hardware const&);
    
    public:
      static Hardware* getInstance() {
         if (!hardware_instance) 
           hardware_instance = new Hardware;
         return hardware_instance;
      }
         
      map<char, string>& get_memory() {return memory;};
      int get_RegisterA() {return RegisterA;}
      int get_RegisterB() {return RegisterB;}
      int get_PC() {return PC;}
      int get_zero_result_bit() {return zero_result_bit;}
      int get_over_flow_bit() {return over_flow_bit;}
      void set_RegisterA(int a) {RegisterA = a;}
      void set_RegisterB(int b) {RegisterB = b;}
      void set_PC(int pc) {PC = pc;}
      void set_zero_result_bit(int zrb) {zero_result_bit = zrb;}
      void set_over_flow_bit(int ofb) {over_flow_bit = ofb;}
};



// print the value on the user's screen.
void print() {
  cout << "RegisterA: " << Hardware::getInstance()->get_RegisterA() << endl;
  cout << "RegisterB: " << Hardware::getInstance()->get_RegisterB() << endl;
  cout << "PC: " << Hardware::getInstance()->get_PC() << endl;
  cout << "Zero_result_bit: " << Hardware::getInstance()->get_zero_result_bit() << endl;
  cout << "Over_flow_bit: " << Hardware::getInstance()->get_over_flow_bit() << endl;
  cout << "Memory: " << '\n';
  for (auto& t : Hardware::getInstance()->get_memory()) {
    cout << t.first << " " <<  t.second << '\n';
  }
}

class Command{
   public:
        virtual void Execute() = 0;
};

class ICommand : public Command{
  public:
    void Execute() {
       ifstream infile(file_name);
       string line;
       int i = 0;
       cout << "Loading SAL program to internal ALI memory: " << endl;
       while (getline(infile, line)) {
        (Hardware::getInstance()->get_memory())['0'+i] = line;
         cout << (Hardware::getInstance()->get_memory())['0'+i] << endl;
         i++;
       }
       Hardware::getInstance()->set_PC(0); // before program starts, set PC = 0.
       Hardware::getInstance()->set_zero_result_bit(0); // set initial zero_result_bit = 0
       Hardware::getInstance()->set_over_flow_bit(0); // set initial over_flow_bit = 0
       cout << endl;
    }
};

class DCommand : public Command{
  public:
    void Execute() {
    int instruction_index = Hardware::getInstance()->get_PC();
    string current_instruction = Hardware::getInstance()->get_memory()['0'+instruction_index];
    if (current_instruction.find("DEC") != std::string::npos) {
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4);
      (Hardware::getInstance()->get_memory()).erase('0'+instruction_index);
      Hardware::getInstance()->get_memory()[current_instruction[0]] = "";
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index); // increase PC value by one 
      print();
    }
    else if (current_instruction.find("LDA") != std::string::npos) {
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4);
      string value = Hardware::getInstance()->get_memory()[current_instruction[0]];
      Hardware::getInstance()->set_RegisterA(stoi(value));
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("LDB") != std::string::npos) {
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4);
      string value = Hardware::getInstance()->get_memory()[current_instruction[0]];
      Hardware::getInstance()->set_RegisterB(stoi(value));
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("LDI") != std::string::npos) { // for example LDI 100
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4); // now current_instruction = 100
      Hardware::getInstance()->set_RegisterA(stoi(current_instruction));  // store 100 into A
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("ST") != std::string::npos) { // for example ST X
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+3); // now current_instruction = X
      int value = Hardware::getInstance()->get_RegisterA();
      Hardware::getInstance()->get_memory()[current_instruction[0]] = to_string(value);
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("XCH") != std::string::npos) {
      int temp = Hardware::getInstance()->get_RegisterA();
      Hardware::getInstance()->set_RegisterA(Hardware::getInstance()->get_RegisterB());
      Hardware::getInstance()->set_RegisterB(temp);
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("JMP") != std::string::npos) { // for example JMP 2
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4); 
      char key = current_instruction[0];
      Hardware::getInstance()->set_PC(key-'0');
      print();
    }
    else if (current_instruction.find("JZS") != std::string::npos) {
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4); 
      char key = current_instruction[0];
      Hardware::getInstance()->set_PC(key-'0');
      print();
    }
    else if (current_instruction.find("JVS") != std::string::npos) {
      current_instruction.erase(current_instruction.begin(), current_instruction.begin()+4); 
      char key = current_instruction[0];
      Hardware::getInstance()->set_PC(key-'0');
      print();
    }
    else if (current_instruction.find("ADD") != std::string::npos) {
      int a = Hardware::getInstance()->get_RegisterA();
      int b = Hardware::getInstance()->get_RegisterB();
      if (!addition_is_overflow(a,b)) {
        int sum = a + b;
        Hardware::getInstance()->set_RegisterA(sum);
        if (sum == 0) Hardware::getInstance()->set_zero_result_bit(1); //if produce zero result, set zero_result_bit = 1
      } else {
        Hardware::getInstance()->set_over_flow_bit(1); // if overflow, set over_flow_bit = 1
      }
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else if (current_instruction.find("HLT") != std::string::npos) {
      instruction_index ++;
      Hardware::getInstance()->set_PC(instruction_index);
      print();
    }
    else {}
    }
    
};

class RCommand : public Command{

  

};

class SCommand : public Command{
};

class QCommand : public Command{
};



class Invoker{
  private:
      std::vector<Command*> _commands;
  public:
        void StoreAndExecute(Command *command)
        {
            if (command){
                _commands.push_back(command);
                command->Execute();
            }
        }
};

Hardware* Hardware::hardware_instance = 0;  // 


int main(){
    std::unique_ptr<Command> i_command (new ICommand);
    std::unique_ptr<Command> d_command (new DCommand);
    std::unique_ptr<Invoker> invoker (new Invoker);
    invoker->StoreAndExecute(i_command.get());
    invoker->StoreAndExecute(d_command.get());
    return 0;
}