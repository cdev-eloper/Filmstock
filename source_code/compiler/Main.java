import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Objects;

public class Main {
    public static boolean debug = false;
    public static boolean info = true;

    public static void compile(String inPath, String outPath) throws IOException {
        Compiler compiler = new Compiler();
        compiler.compile(inPath, outPath);
    }

    public static void assemble(String inPath, String outPath) throws IOException {
        Assembler.assemble(inPath, outPath);
    }

    public static void build(String inPath, String outPath) throws IOException {
        Compiler compiler = new Compiler();

        Compiler.AssemblyInfo info = compiler.compile(inPath);
        FastAssembler f = new FastAssembler();

        f.assemble(info, outPath);
    }

    static void printHelp() {
        System.out.println("You can use multiple commands after one another to queue tasks.");
        System.out.println("-help: Prints this.");
        System.out.println("-build    <inpath> <outpath> : Compiles and assembles a program.");
        System.out.println("-compile  <inpath> <outpath> : Compiles Filmstock into Filmstock Assembly with a debug info file.");
        System.out.println("-assemble <inpath> <outpath> : Assembles Filmstock Assembly into bytecode.");
        System.out.printf("-O         <0-%d / help>      : Sets the optimization level or prints information about optimizations.\n",
                Compiler.maxOptimizations);
        System.out.println("-debug                           : Verbose: When compiling, you can use this flag to " +
                "debug a lot of info about the next Compilation / Assembly.");
        System.out.println("                            -> Used for quick debugging.");
        System.out.println("-no-info                         : Disables building the debug info (.fsdbi) file.");
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            System.err.println("No arguments provided.");
            printHelp();
            return;
        }

        int lastStatement = 0;

        try {
            while (lastStatement < args.length) {
                try {
                    switch (args[lastStatement]) {
                        case "-build":
                            if (args.length - lastStatement < 3) {
                                System.err.println("Usage: -build <inpath> <outpath>");
                                return;
                            }
                            build(args[1+lastStatement], args[2+lastStatement]);
                            lastStatement += 3;
                            debug = false;
                            break;

                        case "-o", "-O":
                            if (args.length - lastStatement < 2) {
                                System.err.println("Usage: -O <0-%d / help> ");
                                return;
                            }

                            if (args[1+lastStatement].toLowerCase().contains("help")) {
                                System.out.println("Optimization Levels:");
                                System.out.println("0   - None");
                                System.out.println("1   - Avoid unnecessary copies.");
                                System.out.println(">=2 - Loop unrolling.");
                            }
                            else {
                                int level;
                                try {
                                    level = Integer.parseInt(args[1 + lastStatement]);
                                } catch (NumberFormatException e) {
                                    System.err.println("Could not parse optimization level!");

                                    throw new RuntimeException(e);
                                }

                                Compiler.optimizationLevel = level;
                            }

                            lastStatement += 2;

                            break;

                        case "-assemble":
                            if (args.length - lastStatement < 3) {
                                System.err.println("Usage: -assemble <inpath> <outpath>");
                                return;
                            }
                            assemble(args[1+lastStatement], args[2+lastStatement]);
                            lastStatement += 3;
                            debug = false;
                            break;

                        case "-compile":
                            if (args.length - lastStatement < 3) {
                                System.err.println("Usage: -compile <inpath> <outpath>");
                                return;
                            }
                            compile(args[1+lastStatement], args[2+lastStatement]);
                            lastStatement += 3;
                            debug = false;
                            break;

                        case "-help":
                            lastStatement++;
                            printHelp();
                            break;

                        case "-debug":
                            debug = true;
                            lastStatement++;
                            break;

                        case "-no-info":
                            info = false;
                            lastStatement++;
                            break;

                        default:
                            System.err.println("Unknown argument: " + args[0]);
                            lastStatement++;
                            printHelp();
                            break;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        } catch (Exception e) {
            if (debug) {
                System.out.println();
                throw e;
            }
            else {
                System.err.println("\n" + e.getMessage());
                System.exit(1);
            }
        }
    }
}
