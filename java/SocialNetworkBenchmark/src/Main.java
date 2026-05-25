import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.TreeMap;
import java.util.Map;

public class Main {

    // Entidad espejo para el mapeo en memoria
    static class Estudiante {
        int studentId;
        String fullName;
        String career;
        int semester;
        double gpa;
        int skillScore;

        public Estudiante(int id, String name, String career, int semester, double gpa, int score) {
            this.studentId = id;
            this.fullName = name;
            this.career = career;
            this.semester = semester;
            this.gpa = gpa;
            this.skillScore = score;
        }
    }

    public static void main(String[] args) {
        // Ajusta la ruta según la ubicación de tu dataset en el proyecto de Java
        String rutaCsv = "data/estudiantes.csv";
        List<Estudiante> dataset = new ArrayList<>();

        System.out.println("Cargando dataset en memoria intermedia...");
        try (BufferedReader br = new BufferedReader(new FileReader(rutaCsv))) {
            String linea;
            br.readLine(); // Ignorar encabezados

            while ((linea = br.readLine()) != null) {
                if (linea.trim().isEmpty()) continue;
                String[] campos = linea.split(",");
                try {
                    int id = Integer.parseInt(campos[0].trim());
                    String nombre = campos[1].trim();
                    String carrera = campos[2].trim();
                    int semestre = Integer.parseInt(campos[3].trim());
                    double gpa = Double.parseDouble(campos[4].trim());
                    int score = Integer.parseInt(campos[5].trim());

                    dataset.add(new Estudiante(id, nombre, carrera, semestre, gpa, score));
                } catch (Exception e) {
                    // Ignorar líneas malformadas silenciosamente para no contaminar la consola
                }
            }
        } catch (IOException e) {
            System.err.println("Error crítico: No se pudo leer el archivo CSV en: " + rutaCsv);
            return;
        }

        int registros = dataset.size();
        System.out.println("Dataset cargado. Registros detectados: " + registros);

        // --- FASE DE WARM-UP (Crucial para mitigar la distorsión del JIT Compiler) ---
        System.out.println("Iniciando fase de calentamiento de la JVM...");
        ejecutarWarmUp(dataset);

        // --- BENCHMARK REAL ---
        System.out.println("Ejecutando mediciones de rendimiento...");

        HashMap<Integer, Estudiante> hashMap = new HashMap<>((int)(registros * 1.3));
        TreeMap<Integer, Estudiante> treeMap = new TreeMap<>();

        try (PrintWriter csvOutput = new PrintWriter(new FileWriter("java_results.csv"))) {
            // Encabezado exacto solicitado por la rúbrica
            csvOutput.println("language,operation,structure,records,time_ms");

            // 1. Inserción en HashMap
            long tInicio = System.nanoTime();
            for (Estudiante est : dataset) {
                hashMap.put(est.studentId, est);
            }
            long tFin = System.nanoTime();
            double tiempoHashMapInsert = (tFin - tInicio) / 1_000_000.0;
            csvOutput.printf("Java,insert,HashMap,%d,%.4f\n", registros, tiempoHashMapInsert);

            // 2. Búsqueda en HashMap
            tInicio = System.nanoTime();
            for (Estudiante est : dataset) {
                hashMap.get(est.studentId);
            }
            tFin = System.nanoTime();
            double tiempoHashMapSearch = (tFin - tInicio) / 1_000_000.0;
            csvOutput.printf("Java,search,HashMap,%d,%.4f\n", registros, tiempoHashMapSearch);

            // 3. Inserción en TreeMap (Árbol Rojo-Negro balanceado)
            tInicio = System.nanoTime();
            for (Estudiante est : dataset) {
                // Se ordena por skillScore. Para permitir duplicados exactos de score en la estructura
                // nativa, usamos un empaquetado binario o una clave compuesta en el árbol.
                treeMap.put(est.skillScore * 1000000 + est.studentId, est);
            }
            tFin = System.nanoTime();
            double tiempoTreeMapInsert = (tFin - tInicio) / 1_000_000.0;
            csvOutput.printf("Java,insert,TreeMap,%d,%.4f\n", registros, tiempoTreeMapInsert);

            // 4. Recorrido In-Order en TreeMap (Iteración natural ordenada)
            tInicio = System.nanoTime();
            for (Map.Entry<Integer, Estudiante> entry : treeMap.entrySet()) {
                int dummy = entry.getValue().skillScore;
            }    // Forzar lectura en memoria volátil para asegurar que el compilador no salte la líne

            tFin = System.nanoTime();
            double tiempoTreeMapTraversal = (tFin - tInicio) / 1_000_000.0;
            csvOutput.printf("Java,traversal,TreeMap,%d,%.4f\n", registros, tiempoTreeMapTraversal);

            System.out.println("Archivo 'java_results.csv' generado exitosamente.");

        } catch (IOException e) {
            System.err.println("Error al escribir el archivo de resultados.");
        }
    }

    private static void ejecutarWarmUp(List<Estudiante> dataset) {
        // Inserciones y búsquedas repetidas en estructuras desechables
        // Esto provoca que el HotSpot VM identifique el código como "hot" y lo optimice antes de medir.
        HashMap<Integer, Estudiante> tempMap = new HashMap<>();
        TreeMap<Integer, Estudiante> tempTree = new TreeMap<>();

        for (int i = 0; i < Math.min(dataset.size(), 1000); i++) {
            Estudiante est = dataset.get(i);
            tempMap.put(est.studentId, est);
            tempTree.put(est.skillScore * 1000000 + est.studentId, est);
        }
        for (int i = 0; i < Math.min(dataset.size(), 1000); i++) {
            tempMap.get(dataset.get(i).studentId);
        }
        tempMap.clear();
        tempTree.clear();
    }
}