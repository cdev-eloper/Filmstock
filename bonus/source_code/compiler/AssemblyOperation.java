import java.util.ArrayList;
import java.util.List;

public class AssemblyOperation {
    Type type;
    public int SIMD = -1;
    Compiler.Variable[] vars;
    Compiler.Line debugLine;

    public List<Integer> labels = new ArrayList<>();

    public String toString() {
        StringBuilder builder = new StringBuilder();

        builder.append(type.name());

        for (Compiler.Variable variable : vars)
            builder.append(" ").append(variable.memoryPosition);

        if ((type == Type.PRINT || type == Type.PRINT_NUMBERS) && vars.length == 1)
            builder.append(" 1");

        return builder.toString();
    }

    public AssemblyOperation(Type type, Compiler.Variable[] vars, Compiler.Line debugLine) {
        this.type = type;
        this.vars = vars;
        this.debugLine = debugLine;
    }
}
