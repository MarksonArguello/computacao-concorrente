// Monitor que implementa a logica do padrao leitores/escritores
class LE {
    private int leit, escr, leitEsc;
    private int number;

    // Construtor
    LE() {
        this.number = 0;
        this.leitEsc = 0;
        this.leit = 0; //leitores lendo (0 ou mais)
        this.escr = 0; //escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores/escritores
    public synchronized void EntraLeitorEscritor (int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
                //if (this.escr > 0) {
                System.out.println ("le.leitorEscritorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++;  //registra que ha mais um leitor/escritor lendo
            System.out.println ("le.leitorEscritorEntrando("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para leitores/escritores
    public synchronized void SaiLeitorEscritor (int id) {
        this.escr--; //registra que um leitorEscritor saiu
        this.notifyAll(); //libera leitor e escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorEscritorSaindo("+id+")");
    }

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            while (this.escr > 0) {
            //if (this.escr > 0) {
                System.out.println ("le.leitorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.leit++;  //registra que ha mais um leitor lendo
            System.out.println ("le.leitorLendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); //libera escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorSaindo("+id+")");
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
            //if ((this.leit > 0) || (this.escr > 0)) {
                System.out.println ("le.escritorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++; //registra que ha um escritor escrevendo
            System.out.println ("le.escritorEscrevendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println ("le.escritorSaindo("+id+")");
    }

    public int getNumber() {
        return number;
    }

    public void setNumber(int number) {
        this.number = number;
    }
}

// Leitor
class Leitor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo
    LE monitor;//objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Leitor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    private boolean isPrime(int number) {
        if (number <= 1 || number % 2 == 0) return false;
        for (int i = 3; i * i <= number; i++) {
            if (number % i == 0) return false;
        }
        return true;
    }

    // Método executado pela thread
    public void run () {

        try {
            for (;;) {
                this.monitor.EntraLeitor(this.id);
                int number = this.monitor.getNumber();
                if (isPrime(number)) {
                    System.out.println("O número " + number + " é primo.");
                } else {
                    System.out.println("O número " + number + " não é primo.");
                }
                this.monitor.SaiLeitor(this.id);
                sleep(this.delay);
            }
        } catch (InterruptedException e) { return; }
    }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo...
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Escritor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        try {
            for (;;) {
                this.monitor.EntraEscritor(this.id);
                this.monitor.setNumber(this.id);
                this.monitor.SaiEscritor(this.id);
                sleep(this.delay); //atraso bobo...
            }
        } catch (InterruptedException e) { return; }
    }
}

//--------------------------------------------------------

// Leitor E Escritor
class EscritorELeitor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo...
    LE monitor; //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    EscritorELeitor (int id, int delayTime, LE m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        try {
            for (;;) {
                this.monitor.EntraLeitorEscritor(this.id);
                int number = this.monitor.getNumber();
                if (number % 2 == 0) {
                    System.out.println("O número " + number + " é par.");
                } else {
                    System.out.println("O número " + number + " é ímpar.");
                }
                this.monitor.setNumber(2 * number);
                this.monitor.SaiLeitorEscritor(this.id);
                sleep(this.delay); //atraso bobo...
            }
        } catch (InterruptedException e) { return; }
    }
}

//--------------------------------------------------------


// Classe principal
class Lab07 {
    static final int L = 4;
    static final int E = 3;
    static final int EL = 3;

    public static void main (String[] args) {
        int i;
        LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
        Leitor[] l = new Leitor[L];       // Threads leitores
        Escritor[] e = new Escritor[E];   // Threads escritores
        EscritorELeitor[] el = new EscritorELeitor[EL];

        //inicia o log de saida
        System.out.println ("import verificaLE");
        System.out.println ("le = verificaLE.LE()");

        for (i=0; i<L; i++) {
            l[i] = new Leitor(i+1, (i+1)*500, monitor);
            l[i].start();
        }
        for (i=0; i<E; i++) {
            e[i] = new Escritor(i+1, (i+1)*500, monitor);
            e[i].start();
        }

        for (i=0; i<EL; i++) {
            el[i] = new EscritorELeitor(i+1, (i+1)*500, monitor);
            el[i].start();
        }

    }
}
