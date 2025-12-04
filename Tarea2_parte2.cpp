#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include <iomanip>

using namespace std;

// marco pagina fisica
struct Frame {
    bool ocupado;       // pagina cargada
    unsigned long vpn; //pagina virtual
    bool use_bit;       // bit referencia reloj
    
    Frame() : ocupado(false), vpn(0), use_bit(false) {}
};

class SimuladorMemoria {
private:
    int num_marcos;
    int page_size;
    int shift_b;           // bits desplazamientoo
    unsigned long mask;   // mask para extraer offset
    bool verbose;
    
    
    vector<Frame> marcos_fisicos;               // memoria fisica
    unordered_map<unsigned long, int> page_table; // tabla paginas VPN 
    int reloj_hand;                             // puntero del reloj

    // estadisticas
    unsigned long total_referencias;
    unsigned long total_fallos;

public:
    SimuladorMemoria(int n, int size, bool v) 
        : num_marcos(n), page_size(size), verbose(v), reloj_hand(0), total_referencias(0), total_fallos(0) {
        

        shift_b = (int)log2(page_size);
        mask = page_size - 1;

        marcos_fisicos.resize(num_marcos);
    }

    // reemplazo reloj
    int obtener_marco_libre_o_victima() {
        while (true) {
            // CASO 1 MARCO LIBRE
            if (!marcos_fisicos[reloj_hand].ocupado) {
                int frame_idx = reloj_hand;
                reloj_hand = (reloj_hand + 1) % num_marcos; 
                return frame_idx;
            }

            // CASO 2 RELOJ
            if (marcos_fisicos[reloj_hand].use_bit == true) {
              
                marcos_fisicos[reloj_hand].use_bit = false;
                reloj_hand = (reloj_hand + 1) % num_marcos;
            } else {
                // VVICTIMA ENCONTRADA
                int victim_idx = reloj_hand;
                reloj_hand = (reloj_hand + 1) % num_marcos; 
                return victim_idx;
            }
        }
    }

    void procesar_direccion(unsigned long direccion_virtual) {
        total_referencias++;

        
        unsigned long offset = direccion_virtual & mask;    //offset = VA & MASK
        unsigned long vpn = direccion_virtual >> shift_b;  // npv = DV >> b

        int marco_idx = -1;
        bool fallo = false;
        string estado = "HIT";

        // verificar tabla paginas VPN
        if (page_table.find(vpn) != page_table.end()) {
            // HIT
            marco_idx = page_table[vpn];
            marcos_fisicos[marco_idx].use_bit = true; // ACTUALIZAR BIT REF
        } else {
            // FALLO
            fallo = true;
            total_fallos++;
            estado = "FALLO";

            // buscar marco
            marco_idx = obtener_marco_libre_o_victima();

            // marco ocupado expula pagina anterior
            if (marcos_fisicos[marco_idx].ocupado) {
                unsigned long vpn_victima = marcos_fisicos[marco_idx].vpn;
                page_table.erase(vpn_victima); 
            }

            // cargar nueva pagina
            marcos_fisicos[marco_idx].ocupado = true;
            marcos_fisicos[marco_idx].vpn = vpn;
            marcos_fisicos[marco_idx].use_bit = true; // nueva pagina comienza con seugnda oportunidad
            
            // actualizar tabla de paginas
            page_table[vpn] = marco_idx;
        }

        // CALCULO DE DIRECCION
        // DF = (marco << b) | offset
        unsigned long direccion_fisica = ((unsigned long)marco_idx << shift_b) | offset;

        // SALIDA VERBOSE
        if (verbose) {
            
            cout << "DV: 0x" << hex << uppercase << direccion_virtual 
                 << " | VPN: 0x" << vpn 
                 << " | Offset: 0x" << offset
                 << " | " << estado 
                 << " | Marco: " << dec << marco_idx 
                 << " | DF: 0x" << hex << uppercase << direccion_fisica << dec << endl;
        }
    }

    void imprimir_resumen() {
        cout << "\nRESULTADOS" << endl;
        cout << "Total referencias: " << total_referencias << endl;
        cout << "Total fallos de pagina: " << total_fallos << endl;
        
        double tasa = 0.0;
        if (total_referencias > 0) {
            tasa = (double)total_fallos / total_referencias;
        }
        cout << "Tasa de fallos: " << fixed << setprecision(4) << tasa << endl;
    }
};

int main(int argc, char* argv[]) {
    // procesar argumentos: ./sim Nmarcos tamañomarco [--verbose] traza.txt
    if (argc < 4) {
        cerr << "Uso: " << argv[0] << " Nmarcos tamano_marco [--verbose] traza.txt" << endl;
        return 1;
    }

    int arg_idx = 1;
    int n_marcos = atoi(argv[arg_idx++]);
    int tam_marco = atoi(argv[arg_idx++]);
    
    bool verbose = false;
    string archivo_traza;

    // verifica flag
    string posible_flag = argv[arg_idx];
    if (posible_flag == "--verbose") {
        verbose = true;
        arg_idx++;
    }
    
    if (arg_idx >= argc) {
        cerr << "Error: falta el archivo de traza." << endl;
        return 1;
    }
    archivo_traza = argv[arg_idx];

    // iniciar simulador
    SimuladorMemoria sim(n_marcos, tam_marco, verbose);
    
    ifstream file(archivo_traza);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << archivo_traza << endl;
        return 1;
    }

    string linea;
    while (file >> linea) {
        try {
            
            unsigned long direccion = stoul(linea, nullptr, 0);
            sim.procesar_direccion(direccion);
        } catch (...) {
            continue; 
        }
    }

    file.close();
    sim.imprimir_resumen();

    return 0;
}
