import React, { useCallback, useEffect, useRef, useState } from "react";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import CircularProgress from "@mui/material/CircularProgress";
import Dialog from "@mui/material/Dialog";
import DialogContent from "@mui/material/DialogContent";
import DialogTitle from "@mui/material/DialogTitle";
import IconButton from "@mui/material/IconButton";
import InputBase from "@mui/material/InputBase";
import Typography from "@mui/material/Typography";
import AddIcon from "@mui/icons-material/Add";
import CloseIcon from "@mui/icons-material/Close";
import FileDownloadOutlinedIcon from "@mui/icons-material/FileDownloadOutlined";
import RemoveIcon from "@mui/icons-material/Remove";
import { useTranslation } from "react-i18next";
import { useDispatch } from "react-redux";
import { useLazyGetFilePreviewMetaQuery } from "../../../store/api-slices/fileApiSlice";
import { instance } from "../../../requests/requests";
import { END_POINT } from "../../../requests/endpoint";
import { getFileDownload } from "../../requests/filerequest";
import { updateCommon } from "../../../store/reducers/common.slice";

interface FilePreviewModalProps {
  fileId: string | null;
  fileName: string;
  isOpen: boolean;
  onClose: () => void;
}

interface PageState {
  url: string | null;
  loading: boolean;
  error: boolean;
}

const EAGER_LOAD_COUNT = 3;

const FilePreviewModal: React.FC<FilePreviewModalProps> = ({
  fileId,
  fileName,
  isOpen,
  onClose,
}) => {
  const { t } = useTranslation();
  const dispatch = useDispatch();
  const [pages, setPages] = useState<PageState[]>([]);
  const [totalPages, setTotalPages] = useState(0);
  const [metaError, setMetaError] = useState<string | null>(null);
  const [metaLoading, setMetaLoading] = useState(false);
  const [zoom, setZoom] = useState(100);
  const [visiblePage, setVisiblePage] = useState(1);
  const [isDownloading, setIsDownloading] = useState(false);
  const [jumpInput, setJumpInput] = useState("1");
  const isJumpInputFocused = useRef(false);

  const objectUrlsRef = useRef<string[]>([]);
  const loadingPagesRef = useRef<Set<number>>(new Set());
  const lazyObserverRef = useRef<IntersectionObserver | null>(null);
  const visibilityObserverRef = useRef<IntersectionObserver | null>(null);
  const pageElemsRef = useRef<Map<number, HTMLDivElement>>(new Map());

  const [triggerMeta] = useLazyGetFilePreviewMetaQuery();

  const cleanupUrls = useCallback(() => {
    objectUrlsRef.current.forEach((url) => URL.revokeObjectURL(url));
    objectUrlsRef.current = [];
  }, []);

  const loadPage = useCallback(async (pageNum: number, id: string) => {
    if (loadingPagesRef.current.has(pageNum)) return;
    loadingPagesRef.current.add(pageNum);

    setPages((prev) => {
      const next = [...prev];
      if (next[pageNum - 1]) {
        next[pageNum - 1] = { ...next[pageNum - 1], loading: true };
      }
      return next;
    });

    try {
      const response = await instance.get(
        `${END_POINT.file}/${id}/preview/${pageNum}`,
        { responseType: "blob", headers: { skipLoading: "true" } },
      );
      const url = URL.createObjectURL(response.data as Blob);
      objectUrlsRef.current.push(url);

      setPages((prev) => {
        const next = [...prev];
        if (next[pageNum - 1]) {
          next[pageNum - 1] = { url, loading: false, error: false };
        }
        return next;
      });
    } catch {
      setPages((prev) => {
        const next = [...prev];
        if (next[pageNum - 1]) {
          next[pageNum - 1] = { url: null, loading: false, error: true };
        }
        return next;
      });
    } finally {
      loadingPagesRef.current.delete(pageNum);
    }
  }, []);

  // Initialise when modal opens
  useEffect(() => {
    if (!isOpen || !fileId) return;

    let cancelled = false;

    const init = async () => {
      setMetaLoading(true);
      setMetaError(null);
      setPages([]);

      setTotalPages(0);
      setVisiblePage(1);
      setZoom(100);
      setJumpInput("1");
      cleanupUrls();
      loadingPagesRef.current.clear();
      pageElemsRef.current.clear();

      try {
        const data = await triggerMeta(fileId).unwrap();
        if (cancelled) return;

        const total = data.total_pages;
        setTotalPages(total);
        const initial = Array.from({ length: total }, () => ({
          url: null,
          loading: false,
          error: false,
        }));
        setPages(initial);

        const eagerCount = Math.min(EAGER_LOAD_COUNT, total);
        for (let i = 1; i <= eagerCount; i++) {
          loadPage(i, fileId);
        }
      } catch (err: unknown) {
        if (cancelled) return;
        const status = (err as { status?: number })?.status;
        if (status === 503) {
          setMetaError(t("text.preview.processing"));
        } else if (status === 404) {
          setMetaError(t("text.preview.notFound"));
        } else {
          setMetaError(t("text.preview.error"));
        }
      } finally {
        if (!cancelled) setMetaLoading(false);
      }
    };

    init();
    return () => {
      cancelled = true;
    };
  }, [isOpen, fileId]);

  // Cleanup when modal closes
  useEffect(() => {
    if (!isOpen) {
      cleanupUrls();
      setPages([]);

      setTotalPages(0);
      setMetaError(null);
      lazyObserverRef.current?.disconnect();
      lazyObserverRef.current = null;
      visibilityObserverRef.current?.disconnect();
      visibilityObserverRef.current = null;
      pageElemsRef.current.clear();
      loadingPagesRef.current.clear();
    }
  }, [isOpen, cleanupUrls]);

  // IntersectionObserver: lazy-load pages 4+ and track visible page number
  useEffect(() => {
    if (totalPages === 0 || !fileId) return;

    if (totalPages > EAGER_LOAD_COUNT) {
      lazyObserverRef.current?.disconnect();
      lazyObserverRef.current = new IntersectionObserver(
        (entries) => {
          entries.forEach((entry) => {
            if (entry.isIntersecting) {
              const pageNum = Number(
                (entry.target as HTMLElement).dataset.page,
              );
              if (pageNum > EAGER_LOAD_COUNT) {
                loadPage(pageNum, fileId);
                lazyObserverRef.current?.unobserve(entry.target);
              }
            }
          });
        },
        { threshold: 0.1, rootMargin: "1200px" },
      );
    }

    visibilityObserverRef.current?.disconnect();
    visibilityObserverRef.current = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting) {
            const pageNum = Number((entry.target as HTMLElement).dataset.page);
            if (pageNum) {
              setVisiblePage(pageNum);
              if (!isJumpInputFocused.current) {
                setJumpInput(String(pageNum));
              }
            }
          }
        });
      },
      { threshold: 0.5 },
    );

    pageElemsRef.current.forEach((el, pageNum) => {
      if (pageNum > EAGER_LOAD_COUNT) {
        lazyObserverRef.current?.observe(el);
      }
      visibilityObserverRef.current?.observe(el);
    });

    return () => {
      lazyObserverRef.current?.disconnect();
      visibilityObserverRef.current?.disconnect();
    };
  }, [totalPages, fileId, loadPage]);

  const registerPageRef = useCallback(
    (pageNum: number) => (el: HTMLDivElement | null) => {
      if (el) {
        pageElemsRef.current.set(pageNum, el);
        if (pageNum > EAGER_LOAD_COUNT && lazyObserverRef.current) {
          lazyObserverRef.current.observe(el);
        }
        if (visibilityObserverRef.current) {
          visibilityObserverRef.current.observe(el);
        }
      } else {
        pageElemsRef.current.delete(pageNum);
      }
    },
    [],
  );

  const handleJumpToPage = useCallback(() => {
    const pageNum = parseInt(jumpInput, 10);
    if (isNaN(pageNum) || pageNum < 1 || pageNum > totalPages) {
      setJumpInput(String(visiblePage));
      return;
    }
    const el = pageElemsRef.current.get(pageNum);
    if (el) {
      el.scrollIntoView({ behavior: "smooth", block: "start" });
    }
  }, [jumpInput, totalPages, visiblePage]);

  const handleDownload = useCallback(async () => {
    if (!fileId || isDownloading) return;
    setIsDownloading(true);
    try {
      const response = await getFileDownload(fileId);
      if (response.status === 200) {
        const url = URL.createObjectURL(response.data as Blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = fileName;
        a.click();
        setTimeout(() => URL.revokeObjectURL(url), 100);
      }
    } catch (error) {
      console.error("Download failed:", error);
      dispatch(
        updateCommon({
          snackBar: {
            isOpen: true,
            message: t("text.download.error"),
            type: "error",
          },
        }),
      );
    } finally {
      setIsDownloading(false);
    }
  }, [fileId, fileName, isDownloading]);

  return (
    <Dialog
      open={isOpen}
      onClose={onClose}
      maxWidth="md"
      fullWidth
      slotProps={{
        paper: {
          sx: {
            height: "90vh",
            display: "flex",
            flexDirection: "column",
          },
        },
      }}
    >
      {/* ── Header ── */}
      <DialogTitle
        sx={{
          display: "flex",
          alignItems: "center",
          justifyContent: "space-between",
          borderBottom: "1px solid #e2e8f0",
          py: 1.25,
          px: 2,
          gap: 2,
          flexShrink: 0,
        }}
      >
        {/* Filename + page counter */}
        <Box
          sx={{ display: "flex", alignItems: "center", gap: 1.5, minWidth: 0 }}
        >
          <Typography
            sx={{
              fontSize: "14px",
              fontWeight: 600,
              color: "#1e293b",
              overflow: "hidden",
              textOverflow: "ellipsis",
              whiteSpace: "nowrap",
            }}
          >
            {fileName}
          </Typography>
          {totalPages > 0 && (
            <Box
              sx={{
                display: "flex",
                alignItems: "center",
                gap: 0.5,
                whiteSpace: "nowrap",
              }}
            >
              <Typography sx={{ fontSize: "12px", color: "#94a3b8" }}>
                Page
              </Typography>
              <InputBase
                type="number"
                value={jumpInput}
                inputProps={{ min: 1, max: totalPages }}
                onChange={(e) => setJumpInput(e.target.value)}
                onKeyDown={(e) => {
                  if (e.key === "Enter") {
                    handleJumpToPage();
                    (e.target as HTMLInputElement).blur();
                  }
                }}
                onFocus={(e) => {
                  isJumpInputFocused.current = true;
                  e.target.select();
                }}
                onBlur={() => {
                  isJumpInputFocused.current = false;
                  setJumpInput(String(visiblePage));
                }}
                sx={{
                  width: "44px",
                  height: "22px",
                  fontSize: "12px",
                  color: "#1e293b",
                  fontFamily: "monospace",
                  border: "1px solid #e2e8f0",
                  borderRadius: "4px",
                  "& input": { padding: "2px 4px", textAlign: "center" },
                  "& input::-webkit-inner-spin-button, & input::-webkit-outer-spin-button":
                    { WebkitAppearance: "none", margin: 0 },
                  "& input[type=number]": { MozAppearance: "textfield" },
                  "&:focus-within": {
                    borderColor: "#93c5fd",
                    outline: "none",
                  },
                }}
              />
              <Typography sx={{ fontSize: "12px", color: "#94a3b8" }}>
                of {totalPages}
              </Typography>
            </Box>
          )}
        </Box>

        {/* Zoom + Download + Close */}
        <Box
          sx={{ display: "flex", alignItems: "center", gap: 1, flexShrink: 0 }}
        >
          <Box
            sx={{
              display: "flex",
              alignItems: "center",
              border: "1px solid #e2e8f0",
              borderRadius: "8px",
              px: 0.5,
            }}
          >
            <IconButton
              size="small"
              onClick={() => setZoom((z) => Math.max(25, z - 25))}
              sx={{ color: "#64748b", p: "4px" }}
            >
              <RemoveIcon sx={{ fontSize: 14 }} />
            </IconButton>
            <Typography
              sx={{
                fontSize: "12px",
                color: "#64748b",
                width: "36px",
                textAlign: "center",
                fontFamily: "monospace",
                userSelect: "none",
              }}
            >
              {zoom}%
            </Typography>
            <IconButton
              size="small"
              onClick={() => setZoom((z) => Math.min(300, z + 25))}
              sx={{ color: "#64748b", p: "4px" }}
            >
              <AddIcon sx={{ fontSize: 14 }} />
            </IconButton>
          </Box>

          <Button
            size="small"
            onClick={handleDownload}
            disabled={isDownloading}
            startIcon={
              isDownloading ? (
                <CircularProgress size={13} color="inherit" />
              ) : (
                <FileDownloadOutlinedIcon sx={{ fontSize: 14 }} />
              )
            }
            variant="outlined"
            sx={{
              fontSize: "12px",
              color: "#64748b",
              borderColor: "#e2e8f0",
              borderRadius: "8px",
              px: 1.5,
              py: 0.5,
              textTransform: "none",
              fontWeight: 500,
              "&:hover": { borderColor: "#cbd5e1", backgroundColor: "#f8fafc" },
            }}
          >
            Download
          </Button>

          <IconButton size="small" onClick={onClose} sx={{ color: "#64748b" }}>
            <CloseIcon fontSize="small" />
          </IconButton>
        </Box>
      </DialogTitle>

      {/* ── Page list ── */}
      <DialogContent
        sx={{
          flex: 1,
          overflow: "auto",
          display: "flex",
          flexDirection: "column",
          alignItems: "center",
          gap: 2,
          p: 3,
          backgroundColor: "#f8fafc",
        }}
      >
        {metaLoading && (
          <Box
            sx={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              flex: 1,
              minHeight: "200px",
            }}
          >
            <CircularProgress />
          </Box>
        )}

        {metaError && !metaLoading && (
          <Box
            sx={{
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
              flex: 1,
              minHeight: "200px",
            }}
          >
            <Typography sx={{ color: "#ef4444", fontSize: "14px" }}>
              {metaError}
            </Typography>
          </Box>
        )}

        {!metaLoading &&
          !metaError &&
          pages.map((page, idx) => {
            const pageNum = idx + 1;
            return (
              <Box
                key={pageNum}
                ref={registerPageRef(pageNum)}
                data-page={pageNum}
                sx={{
                  width: `${750 * (zoom / 100)}px`,
                  maxWidth: "100%",
                  flexShrink: 0,
                  borderRadius: "6px",
                  overflow: "hidden",
                  boxShadow: "0 1px 4px rgba(0,0,0,0.12)",
                  backgroundColor: "#fff",
                }}
              >
                <Box
                  sx={{
                    aspectRatio: "1 / 1.414",
                    display: "flex",
                    alignItems: "center",
                    justifyContent: "center",
                  }}
                >
                  {page.loading && <CircularProgress size={28} />}
                  {page.error && (
                    <Typography sx={{ fontSize: "13px", color: "#ef4444" }}>
                      {t("text.preview.pageError")}
                    </Typography>
                  )}
                  {page.url && (
                    <img
                      src={page.url}
                      alt={`Page ${pageNum}`}
                      style={{ width: "100%", display: "block" }}
                    />
                  )}
                  {!page.loading && !page.error && !page.url && (
                    <CircularProgress size={20} sx={{ opacity: 0.35 }} />
                  )}
                </Box>
              </Box>
            );
          })}
      </DialogContent>
    </Dialog>
  );
};

export default FilePreviewModal;
