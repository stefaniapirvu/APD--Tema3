# APD--Tema3
Calcule colaborative ˆın sisteme distribuite


Pentru rezolvarea temei am folosit citirea din fisiere din 
laboratorul 10 si o parte din implementarea algoritmului inel
din laboratorul 8.

In main
      Am verificat in rpimul rand daca procesul este unul parinte 
    (0,1,2,3) sau este de tip worker.
    Proces parinte:
        Apelez funcia read_workers ce citeste din fisierul corespunzator
        si retin numarul de workers subordonati si care sunt acestia.
        
        Construiect topologia apeland functia find_topology prezentata 
        mai jos. In cazul in care clusterul 1 este izolat, (err dat ca
        parametru este 2), atunci apelez functia find_topology_err.

        Dupa ce am aflat topologia o afisez si o transmit proceselor worker

        Daca rank-ul procesului este 0, acesta va apela functia split_work.
        Functia imparte un vector in subvectori si ii transmite celorlalte
        procese.
        Daca procesul nu are rank 0, atunci apeleaza functia tasks_lvl1,
        functia in care un cluster primeste un subvector si il imparte in 
        mod egal proceselor worker subordonate.


    Proces worker:    
        Primeste topologia si retine drept parinte procesul de la care a 
        primit mesajele in functia receive_workers.
        Apeleaza functia tasks_lvl2 in care primeste subvectorul ce trebuie
        procesat, il proceseaza si il trimite inapoi modificat.

Functia find_topology 
    Aloc memorie pentru o matrice ce va reprezenta topologia.
    Rank-ul curent isi trece in topologie datele sale, adica lista sa 
    de workers.

    Mai departe se transmit datele( partile cunoscute din topologie)
    intre procese astfel:
    1 -> 2 top[1];
    2 -> 3 top[1], top[2];
    3 -> 0 top[1], top[2], top[3];
    0 -> 3 top[0];
    3 -> 2 top[0], top[3];
    2 -> 1 top[0], top[2], top[3];  
    Astfel toate procesele vor cunoaste topologia.
    Asemanator am procedat si pentru cazul in care clusterul 1 este 
    izolat, doar ca el nu a mai trimis si nu i s-au mai trimis date

Functia split_work
    Functia este apelata doar de catre procesul cu rank 0.
    Acesta calculeaza numarul de procese de tip worker pe care le are 
    fiecare cluster si afla cate elemente din vector ar trebui sa fie
    atribuite punui proces worker.
    Astfel realizeaza si impartirea vectorului intre clustere in functie
    de numarul de workers pe care ii coordoneaza.
    Dupa ce a realizat impartirea, o trimite procesului cu rank 3, care isi
    pastreaza datele necesare lui si pe restul le trimite mai departe catre
    procesul urmator.

Functia my_func
    Functia este apelata doar de clustere si are rolul de a imparti
    subvectorul corespunzator tuturor proceselor de tip worker, in mod
    egal si de a le trimite aceasta impartire.
    Dupa impartire, clusterul primeste de la fiecare proces subordonat
    vectorul modificat. Dupa ce primeste rezultatul de la fiecare 
    proces worker, le asambleaza intr-un singur vector.

    Procesul cu rank 0 primeste de la celelalte proces parinte acesti
    vectori rezultati si ii asambleaza in rezultatul final pe care il afiseaza.
    Si aici, in comunicarea dintre procese se tine cont daca 
    clusterul 1 este izolat sau nu.     
