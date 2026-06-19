import React, { useCallback, useEffect, useState } from "react";
import { FieldError, useForm } from "react-hook-form";
import { useDispatch } from "react-redux";
import Dialog from "@mui/material/Dialog";
import DialogTitle from "@mui/material/DialogTitle";
import DialogContent from "@mui/material/DialogContent";
import DialogActions from "@mui/material/DialogActions";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import Typography from "@mui/material/Typography";
import LinearProgress from "@mui/material/LinearProgress";
import IconButton from "@mui/material/IconButton";
import CloseIcon from "@mui/icons-material/Close";
import AutoFixHighIcon from "@mui/icons-material/AutoFixHigh";
import { useTranslation } from "react-i18next";
import { CommonUploadFile } from "../../../components/form/CommonUploadFile.tsx";
import { resetValidation } from "../../../store/reducers/fileValidateStatus.slice.ts";
import {
  CustomizeItem,
  CustomizeType,
} from "../../../store/api-slices/promptApiSlice.ts";
import { ACCEPT_CUSTOMIZED_FILE_FORMAT } from "../../../constants/context.ts";

interface CustomizeFormModalProps {
  open: boolean;
  onClose: () => void;
  type: CustomizeType;
  item?: CustomizeItem;
  isSubmitting: boolean;
  onSubmit: (formData: FormData) => Promise<void>;
}

/** Extract `name` and `description` from a YAML frontmatter block. */
const parseFrontmatter = (
  content: string,
): { name?: string; description?: string } => {
  const match = content.match(/^---\s*\r?\n([\s\S]*?)\r?\n---/);
  if (!match) return {};
  const block = match[1];
  const nameMatch = block.match(/^name:\s*(.+)$/m);
  const descMatch = block.match(/^description:\s*(.+)$/m);
  return {
    name: nameMatch?.[1]?.trim(),
    description: descMatch?.[1]?.trim(),
  };
};

const getExtension = (name: string): string => {
  const parts = name.split(".");
  return parts.length > 1 ? parts[parts.length - 1].toLowerCase() : "";
};

const readAsText = (file: File): Promise<string> =>
  new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = (e) => resolve(e.target?.result as string);
    reader.onerror = reject;
    reader.readAsText(file);
  });

type FileForm = { file: File | undefined };

export const CustomizeFormModal: React.FC<CustomizeFormModalProps> = ({
  open,
  onClose,
  type,
  item,
  isSubmitting,
  onSubmit,
}) => {
  const { t } = useTranslation();
  const dispatch = useDispatch();
  const isEdit = !!item;

  const [title, setTitle] = useState(item?.name ?? "");
  const [description, setDescription] = useState(item?.description ?? "");
  const [acceptedFileName, setAcceptedFileName] = useState(
    item?.prompt_file_info?.prompt_file_name ?? "",
  );
  const [autoExtracted, setAutoExtracted] = useState(false);
  const [fieldErrors, setFieldErrors] = useState<Record<string, string>>({});

  const {
    control,
    getValues,
    reset: resetFileForm,
  } = useForm<FileForm>({
    defaultValues: { file: undefined },
  });

  useEffect(() => {
    if (open) {
      setTitle(item?.name ?? "");
      setDescription(item?.description ?? "");
      setAcceptedFileName(item?.prompt_file_info?.prompt_file_name ?? "");
      setAutoExtracted(false);
      setFieldErrors({});
      resetFileForm({ file: undefined });
    } else {
      dispatch(resetValidation());
    }
  }, [open, item, resetFileForm, dispatch]);

  const handleClose = useCallback(() => {
    if (!isSubmitting) onClose();
  }, [isSubmitting, onClose]);

  const handleFileChange = useCallback(
    async (received: File | File[] | undefined) => {
      const file = Array.isArray(received) ? received[0] : received;

      if (!file) {
        setAcceptedFileName("");
        setAutoExtracted(false);
        return;
      }

      const ext = getExtension(file.name);
      if (!["yaml", "yml", "md"].includes(ext)) {
        setFieldErrors((prev) => ({
          ...prev,
          file: t("customize.invalidFileType"),
        }));
        setAcceptedFileName("");
        return;
      }

      try {
        const content = await readAsText(file);
        setAcceptedFileName(file.name);
        setFieldErrors((prev) => ({ ...prev, file: "" }));

        const fm = parseFrontmatter(content);
        if (fm.name || fm.description) {
          setAutoExtracted(true);
          if (fm.name) setTitle(fm.name);
          if (fm.description) setDescription(fm.description);
        } else {
          setAutoExtracted(false);
        }
      } catch {
        setFieldErrors((prev) => ({
          ...prev,
          file: t("customize.fileReadError"),
        }));
        setAcceptedFileName("");
      }
    },
    [t],
  );

  const validate = (): boolean => {
    const newErrors: Record<string, string> = {};
    if (!title.trim()) newErrors.title = t("customize.titleRequired");
    if (!description.trim())
      newErrors.description = t("customize.descriptionRequired");
    if (!isEdit && !acceptedFileName) {
      newErrors.file = fieldErrors.file || t("customize.fileRequired");
    } else if (fieldErrors.file) {
      newErrors.file = fieldErrors.file;
    }
    setFieldErrors(newErrors);
    return Object.keys(newErrors).length === 0;
  };

  const handleSubmit = async () => {
    if (!validate()) return;

    const raw = getValues("file");
    const file = Array.isArray(raw) ? raw[0] : raw;

    const formData = new FormData();
    formData.append("name", title.trim());
    formData.append("description", description.trim());
    formData.append("type", type);
    if (file) formData.append("file", file);

    try {
      await onSubmit(formData);
      handleClose();
    } catch (e) {
      console.error("Failed to save customize item:", e);
    }
  };

  const fileFieldError: FieldError | undefined = fieldErrors.file
    ? ({ type: "required", message: fieldErrors.file } as FieldError)
    : undefined;

  return (
    <Dialog
      open={open}
      onClose={handleClose}
      maxWidth="sm"
      fullWidth
      PaperProps={{
        sx: { borderRadius: 3, backgroundColor: "background.paper" },
      }}
    >
      {isSubmitting && (
        <LinearProgress
          sx={{
            position: "absolute",
            top: 0,
            left: 0,
            right: 0,
            borderRadius: "12px 12px 0 0",
          }}
        />
      )}

      <DialogTitle
        sx={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          pb: 0.5,
          pt: 2.5,
          px: 3,
        }}
      >
        <Typography
          sx={{ fontSize: "17px", fontWeight: 700, color: "text.primary" }}
        >
          {isEdit
            ? t("customize.editTitle")
            : t(
                type === "rule"
                  ? "customize.createRule"
                  : "customize.createPrompt",
              )}
        </Typography>
        <IconButton size="small" onClick={handleClose} disabled={isSubmitting}>
          <CloseIcon sx={{ fontSize: 18 }} />
        </IconButton>
      </DialogTitle>

      <DialogContent sx={{ px: 3, pt: 2, pb: 1 }}>
        <CommonUploadFile
          control={control}
          name="file"
          customAccept={ACCEPT_CUSTOMIZED_FILE_FORMAT}
          onFileChange={handleFileChange}
          errors={fileFieldError}
          helperText={
            fieldErrors.file ? fieldErrors.file : t("customize.frontmatterNote")
          }
        />

        {autoExtracted && (
          <Box
            sx={{
              display: "flex",
              alignItems: "center",
              gap: 0.75,
              mt: 1.5,
              mb: 0.5,
              color: "#10B981",
            }}
          >
            <AutoFixHighIcon sx={{ fontSize: 15 }} />
            <Typography sx={{ fontSize: "12px", fontWeight: 500 }}>
              {t("customize.frontmatterExtracted")}
            </Typography>
          </Box>
        )}

        <TextField
          label={t("customize.titleLabel")}
          value={title}
          onChange={(e) => {
            setTitle(e.target.value);
            setFieldErrors((p) => ({ ...p, title: "" }));
          }}
          error={!!fieldErrors.title}
          helperText={fieldErrors.title}
          fullWidth
          size="small"
          required
          placeholder={t("customize.titlePlaceholder")}
          sx={{ mt: 2, mb: 2 }}
        />

        <TextField
          label={t("customize.descriptionLabel")}
          value={description}
          onChange={(e) => {
            setDescription(e.target.value);
            setFieldErrors((p) => ({ ...p, description: "" }));
          }}
          error={!!fieldErrors.description}
          helperText={fieldErrors.description}
          fullWidth
          size="small"
          required
          multiline
          rows={3}
          placeholder={t("customize.descriptionPlaceholder")}
        />
      </DialogContent>

      <DialogActions sx={{ px: 3, pb: 2.5, pt: 1.5, gap: 1 }}>
        <Button
          variant="outlined"
          onClick={handleClose}
          disabled={isSubmitting}
          sx={{ textTransform: "none" }}
        >
          {t("labelButton.cancel")}
        </Button>
        <Button
          variant="contained"
          onClick={handleSubmit}
          disabled={isSubmitting}
          sx={{ textTransform: "none" }}
        >
          {isEdit ? t("customize.update") : t("customize.create")}
        </Button>
      </DialogActions>
    </Dialog>
  );
};
