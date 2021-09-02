class Contadora extends Thread{
    private int cnt;
    private int nThreads;
    private int id;
    private int[] array;
    private int size;


    public Contadora(int tid, int[] array, int size, int nThreads) {
        this.id    = tid;
        this.nThreads = nThreads;
        this.array = array;
        this.size  = size;
        this.cnt = 0;
    }


    @Override
    public void run() {
        System.out.printf("Thread %d iniciou\n", id);
        for (int ind = id; ind < size; ind += nThreads) {
            if (array[ind] % 2 == 0) {
                cnt++;
            }
        }
        System.out.printf("Thread %d terminou\n", id);
    }

    public int getCnt() {
        return cnt;
    }
}

public class Lab06 {
    static final int MOD = 101;

    /**
     * Conta números de pares no array de forma sequencial.
     * @param array
     * @return nº de pares no array
     */
    static int cntSeq(int[] array) {
        int cnt = 0;
        for (int i : array) {
            if (i % 2 == 0)
                cnt++;
        }
        return cnt;
    }

    /**
     * Inicializa o array.
     * @param size tamanho do array
     * @return array inicializado
     */
    static int[] initArray(int size) {
        int[] array = new int[size];
        for (int i = 0; i < size; i++) {
            array[i] = i % MOD;
        }
        return array;
    }

    /**
     * inicializa array de threads.
     *
     * @param nThreads numero de threads
     * @param array array com os valores
     * @param size tamnho do array
     * @return array de threads inicializado
     */
    static Contadora[] initContadoraArray(int nThreads, int[] array, int size) {
        Contadora[] contadoras =  new Contadora[nThreads];

        for (int i = 0; i < nThreads; i++) {
            contadoras[i] = new Contadora(i, array, size, nThreads);
        }
        return contadoras;
    }


    /**
     * Roda as threads
     * @param nThreads
     * @param contadoras
     */
    static void runThreads(int nThreads, Contadora[] contadoras) {
        for (int i = 0; i < nThreads; i++) {
            contadoras[i].run();
        }
    }

    /**
     *
     * @param nThreads
     * @param contadoras
     * @return número total de numeros pares no array.
     */
    static int esperaThreads(int nThreads, Contadora[] contadoras) {
        int cnt = 0;
        for (int i = 0; i < nThreads; i++) {
            try {
                contadoras[i].join();
                cnt += contadoras[i].getCnt();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return cnt;
    }

    /**
     * Verifica se a função sequencial e a concorrente obtiveram os mesmos valores
     * @param cntSeq
     * @param cntConc
     */
    static void verificaErro(int cntSeq, int cntConc) {
        if (cntSeq == cntConc) {
            System.out.println("Tanto a forma sequencial quanto a concorrente encontraram: "+ cntConc);
        } else {
            System.out.println("Valores diferentes");
            System.out.println("Sequencial: " + cntSeq);
            System.out.println("Concorrente: " + cntConc);
        }
    }

    public static void main(String[] args) {
        int size     = (int)1e8;
        int nThreads = 5;
        int[] array  = initArray(size);
        int cntSeq   = cntSeq(array);

        Contadora[] contadoras = initContadoraArray(nThreads, array, size);

        runThreads(nThreads, contadoras);

        int cntConc = esperaThreads(nThreads, contadoras);

        verificaErro(cntSeq, cntConc);

    }
}
