import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.util.List;

public class FastAssembler {
    public void assemble(Compiler.AssemblyInfo info, String outPath) {
        // 4 bytes is the size of the film
        // Then 12 bytes per constant
        // Then a 64 byte empty margin
        // Then 16 bytes per operation
        byte[] toSave = new byte[4 + info.positions().size() * 12 + 64 + 16 * info.operations().size()];

        // Write size
        writeInt(toSave, 0, info.size());

        for (int i = 0; i < info.positions().size(); i++) {
            int pos = info.positions().get(i);
            double val = info.values().get(i);

            final int base = 4 + i * 12;

            writeInt(toSave, base, pos);
            writeDouble(toSave, base + 4, val);
        }

        Compiler.Variable placeholder = new Compiler.Variable("placeholder");

        placeholder.memoryPosition = 1;

        for (int i = 0; i < info.operations().size(); i++){
            final int base = 4 + info.positions().size() * 12 + 64 + i * 16;
            AssemblyOperation op = info.operations().get(i);

            Compiler.Variable[] vars = op.vars;
            if ((op.type == Type.PRINT || op.type == Type.PRINT_NUMBERS) && vars.length == 1) {
                vars = new Compiler.Variable[] { vars[0],  placeholder};
            }

            // Write type
            if (op.SIMD > 0) {
                writeInt(toSave, base, op.SIMD);
            }
            else {
                writeInt(toSave, base, op.type.ordinal());
            }
            for (int j = 0; j < vars.length; j++) {
                writeInt(toSave, base + 4 + j * 4, vars[j].memoryPosition);
            }
        }

        for (int i = 4 + info.positions().size() * 12; i < 4 + info.positions().size() * 12 + 64; i++) {
            toSave[i] = (byte) -1;
        }

        try (FileOutputStream fos = new FileOutputStream(outPath)) {
            fos.write(toSave);
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (Main.info) {
            StringBuilder debugInfo = new StringBuilder();

            for (AssemblyOperation op : info.operations()) {
                Compiler.Line l = op.debugLine;

                String code = l.code.replace("\n", "");
                code = code.substring(0, Math.min(80, code.length()));

                String id = l.id.replace("\n", "");
                id = id.substring(Math.max(0, id.length() - 1000));

                debugInfo.append(id).append("\n")
                        .append(code).append("\n");
            }

            String debugPath = outPath.replaceFirst("[.][^.]+$", "") + ".fsdbi";

            try (FileOutputStream fos = new FileOutputStream(debugPath)) {
                fos.write(debugInfo.toString().getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    void writeInt(byte[] arr, int index, int value) {
        arr[index]     = (byte) (value);
        arr[index + 1] = (byte) (value >> 8);
        arr[index + 2] = (byte) (value >> 16);
        arr[index + 3] = (byte) (value >> 24);
    }

    void writeDouble(byte[] arr, int index, double value) {
        long bits = Double.doubleToRawLongBits(value);
        arr[index]     = (byte) (bits);
        arr[index + 1] = (byte) (bits >> 8);
        arr[index + 2] = (byte) (bits >> 16);
        arr[index + 3] = (byte) (bits >> 24);
        arr[index + 4] = (byte) (bits >> 32);
        arr[index + 5] = (byte) (bits >> 40);
        arr[index + 6] = (byte) (bits >> 48);
        arr[index + 7] = (byte) (bits >> 56);
    }
}
